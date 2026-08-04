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
    	NB_CORE_ASSERT(m_VertexLibrary, "Could not compile Metal vertex shader {}: {}", m_Name, error->localizedDescription()->utf8String());
    	m_VertexFunction = m_VertexLibrary->newFunction(NS::String::string("vertexMain", NS::ASCIIStringEncoding));

    	m_FragmentLibrary = device->newLibrary(NS::String::string(fragmentSource.c_str(), NS::ASCIIStringEncoding), compileOptions, &error);
    	NB_CORE_ASSERT(m_FragmentLibrary, "Could not compile Metal fragment shader {}: {}", m_Name, error->localizedDescription()->utf8String());
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

    	Slang::ComPtr<slang::ISession> session;
    	s_GlobalSession->createSession(sessionDesc, session.writeRef());

    	session->createCompileRequest(m_CompileRequest.writeRef());

    	int translationUnitIdx = m_CompileRequest->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, m_Name.c_str());
    	m_CompileRequest->addTranslationUnitSourceString(translationUnitIdx, m_Name.c_str(), source.c_str());

    	int vertexEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "vertexMain", SLANG_STAGE_VERTEX);
    	int fragmentEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "fragmentMain", SLANG_STAGE_FRAGMENT);

    	const SlangResult vertexCompileResult = m_CompileRequest->compile();
    	NB_CORE_ASSERT(SLANG_SUCCEEDED(vertexCompileResult), "Slang compilation failed !");

    	m_CompileRequest->getProgramWithEntryPoints(m_ProgramReflection.writeRef());

    	size_t outputSize = 0;

    	std::string vertexSource = static_cast<const char*>(m_CompileRequest->getEntryPointCode(vertexEntryPointIdx, &outputSize));
    	std::string fragmentSource = static_cast<const char*>(m_CompileRequest->getEntryPointCode(fragmentEntryPointIdx, &outputSize));

    	CompileMSL(device, vertexSource, fragmentSource);
    }

	uint32_t Metal3Shader::GetUniformBlockLocation(const std::string& uniformBlockName)
	{
    	slang::ProgramLayout* programLayout = m_ProgramReflection->getLayout();
    	uint32_t paramCount = programLayout->getParameterCount();

    	uint32_t paramIndex = 0;
    	slang::VariableLayoutReflection* uniformLayout = nullptr;
    	while (paramIndex < paramCount && !uniformLayout)
    	{
    		slang::VariableLayoutReflection* paramLayout = programLayout->getParameterByIndex(paramIndex);
    		if (strcmp(paramLayout->getName(), uniformBlockName.c_str()) == 0) uniformLayout = paramLayout;
    		paramIndex++;
    	}

    	if (uniformLayout)
    	{
    		return uniformLayout->getOffset(slang::ParameterCategory::ConstantBuffer);
    	}
    	return 0;
	}

	uint32_t Metal3Shader::GetVertexInputLocation(const std::string& vertexInputName)
	{
    	slang::ProgramLayout* programLayout = m_ProgramReflection->getLayout(0);
    	uint32_t paramCount = programLayout->getParameterCount();

    	uint32_t paramIndex = 0;
    	slang::VariableLayoutReflection* bufferLayout = nullptr;
    	while (paramIndex < paramCount && !bufferLayout)
    	{
    		slang::VariableLayoutReflection* paramLayout = programLayout->getParameterByIndex(paramIndex);
    		if (strcmp(paramLayout->getName(), vertexInputName.c_str()) == 0) bufferLayout = paramLayout;
    		paramIndex++;
    	}

    	if (bufferLayout)
    	{
    		slang::TypeLayoutReflection* typeLayout = bufferLayout->getTypeLayout();
    		slang::VariableLayoutReflection* firstParam = typeLayout->getFieldByIndex(0);
    		return firstParam->getOffset(slang::ParameterCategory::VertexInput);
    	}
    	return 0;
	}
}
