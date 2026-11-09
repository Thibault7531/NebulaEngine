#include "Metal3Shader.h"

#include "Metal3Texture.h"

namespace Nebula::Graphics
{
	bool Metal3Shader::s_IsSlangGlobalSessionCreated = false;
	Slang::ComPtr<slang::IGlobalSession> Metal3Shader::s_GlobalSession = nullptr;

    Metal3Shader::Metal3Shader(MTL::Device* device, const std::filesystem::path& path)
    {
    	m_Name = path.stem().string();
		std::string source = ReadFile(path);
    	CompileSlang(device, source);
    }

    Metal3Shader::~Metal3Shader()
    {
    	m_VertexFunction->release();
    	m_VertexLibrary->release();
    	m_FragmentFunction->release();
    	m_FragmentLibrary->release();

    	m_ProgramReflection = nullptr;
    	m_Session = nullptr;
    }

    std::string Metal3Shader::ReadFile(const std::filesystem::path& path)
    {
    	std::string result;
    	std::ifstream in(path, std::ios::in | std::ios::binary);
    	if (in)
    	{
    		in.seekg(0, std::ios::end);
    		result.resize(in.tellg());
    		in.seekg(0, std::ios::beg);
    		in.read(&result[0], result.size());
    		in.close();
    	}
    	else
    	{
    		NB_CORE_ERROR("Could not open file '{0}'", path.string());
    	}

    	return result;
    }

    void Metal3Shader::CompileMSL(MTL::Device* device, const std::string& vertexSource, const std::string& fragmentSource)
    {
    	MTL::CompileOptions* compileOptions = MTL::CompileOptions::alloc()->init();
    	compileOptions->setCompileSymbolVisibility(MTL::CompileSymbolVisibilityDefault);
    	NS::Error* error;

    	m_VertexLibrary = device->newLibrary(NS::String::string(vertexSource.c_str(), NS::ASCIIStringEncoding), compileOptions, &error);
    	NB_CORE_ASSERT(m_VertexLibrary, std::format("Could not compile Metal vertex shader {}: {}", m_Name, error->localizedDescription()->utf8String()))
    	m_VertexFunction = m_VertexLibrary->newFunction(NS::String::string("vertexMain", NS::ASCIIStringEncoding));

    	m_FragmentLibrary = device->newLibrary(NS::String::string(fragmentSource.c_str(), NS::ASCIIStringEncoding), compileOptions, &error);
    	NB_CORE_ASSERT(m_FragmentLibrary, std::format("Could not compile Metal fragment shader {}: {}", m_Name, error->localizedDescription()->utf8String()))
    	m_FragmentFunction = m_FragmentLibrary->newFunction(NS::String::string("fragmentMain", NS::ASCIIStringEncoding));

    	compileOptions->release();
    }

	void Metal3Shader::CompileSlang(MTL::Device* device, const std::string& source)
    {
    	if (!s_IsSlangGlobalSessionCreated)
    	{
    		slang::createGlobalSession(s_GlobalSession.writeRef());

    		s_IsSlangGlobalSessionCreated = true;
    	}

    	slang::SessionDesc sessionDesc = {};
    	slang::TargetDesc targetDesc = {};
    	targetDesc.format = SLANG_METAL;
    	targetDesc.profile = s_GlobalSession->findProfile("metallib_2_3");

    	sessionDesc.targets = &targetDesc;
    	sessionDesc.targetCount = 1;

    	s_GlobalSession->createSession(sessionDesc, m_Session.writeRef());

    	Slang::ComPtr<slang::ICompileRequest> compileRequest;
    	m_Session->createCompileRequest(compileRequest.writeRef());

    	int translationUnitIdx = compileRequest->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, m_Name.c_str());
    	compileRequest->addTranslationUnitSourceString(translationUnitIdx, m_Name.c_str(), source.c_str());

    	int vertexEntryPointIdx = compileRequest->addEntryPoint(translationUnitIdx, "vertexMain", SLANG_STAGE_VERTEX);
    	int fragmentEntryPointIdx = compileRequest->addEntryPoint(translationUnitIdx, "fragmentMain", SLANG_STAGE_FRAGMENT);

    	const SlangResult vertexCompileResult = compileRequest->compile();
    	NB_CORE_ASSERT(SLANG_SUCCEEDED(vertexCompileResult), "Slang compilation failed !");

    	compileRequest->getProgramWithEntryPoints(m_ProgramReflection.writeRef());

    	size_t outputSize = 0;

    	std::string vertexSource = static_cast<const char*>(compileRequest->getEntryPointCode(vertexEntryPointIdx, &outputSize));
    	std::string fragmentSource = static_cast<const char*>(compileRequest->getEntryPointCode(fragmentEntryPointIdx, &outputSize));

    	CompileMSL(device, vertexSource, fragmentSource);
    	GenerateLayout();
    }

	void Metal3Shader::GenerateLayout()
    {
    	slang::ProgramLayout* programLayout = m_ProgramReflection->getLayout();
    	uint32_t paramCount = programLayout->getParameterCount();
    	uint32_t maxBinding = 0;

    	for (uint32_t i = 0; i < paramCount; i++)
    	{
    		auto varLayout = programLayout->getParameterByIndex(i);
    		auto var = varLayout->getVariable();
    		auto typeLayout = varLayout->getTypeLayout();

    		ResourceBinding res{};
    		res.Name = var->getName();
    		res.Size = typeLayout->getSize();

    		switch (typeLayout->getKind())
    		{
    		case slang::TypeReflection::Kind::ConstantBuffer:
    		case slang::TypeReflection::Kind::ParameterBlock:
    			{
    				res.Type = ResourceType::UniformBuffer;
    				res.Binding = varLayout->getOffset(slang::ParameterCategory::ConstantBuffer);
    				res.Set = varLayout->getBindingSpace(slang::ParameterCategory::ConstantBuffer);
    				maxBinding = std::max(res.Binding, maxBinding);
    				break;
    			}

    		case slang::TypeReflection::Kind::Resource:
    			{
    				res.Type = ResourceType::Texture;
    				res.Binding = varLayout->getOffset(slang::ParameterCategory::ConstantBuffer);
    				res.Set = varLayout->getBindingSpace(slang::ParameterCategory::ConstantBuffer);
    				maxBinding = std::max(res.Binding, maxBinding);
    				break;
    			}

    		case slang::TypeReflection::Kind::SamplerState:
    			{
    				res.Type = ResourceType::Sampler;
    				res.Binding = varLayout->getOffset(slang::ParameterCategory::ConstantBuffer);
    				res.Set = varLayout->getBindingSpace(slang::ParameterCategory::ConstantBuffer);
    				maxBinding = std::max(res.Binding, maxBinding);
    				break;
    			}

    		default: continue;
    		}

    		m_Layout.ResourceIndex[res.Name] = m_Layout.ResourceIndex.size();
    		m_Layout.Resources.push_back(res);
    		m_Layout.VertexBufferLocation = maxBinding + 1;
    	}
    }

	uint32_t Metal3Shader::GetUniformBlockLocation(const std::string& uniformBlockName)
	{
    	if (m_Layout.ResourceIndex.find(uniformBlockName) == m_Layout.ResourceIndex.end())
		{
			return 0;
		}
    	ResourceBinding resource = m_Layout.Resources[m_Layout.ResourceIndex[uniformBlockName]];
    	return resource.Binding;
	}

	uint32_t Metal3Shader::GetVertexInputLocation()
	{
    	return m_Layout.VertexBufferLocation;
	}
}
