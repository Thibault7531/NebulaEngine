#include "Metal3ShaderModule.h"

#include "Metal3Texture.h"

namespace Nebula::Graphics
{
	bool Metal3ShaderModule::s_IsSlangGlobalSessionCreated = false;
	Slang::ComPtr<slang::IGlobalSession> Metal3ShaderModule::s_GlobalSession = nullptr;

    Metal3ShaderModule::Metal3ShaderModule(MTL::Device* device, const std::filesystem::path& path)
    {
    	m_Name = path.stem().string();
    	CompileSlang(device, path);
    }

    Metal3ShaderModule::~Metal3ShaderModule()
    {
    	m_Library->release();
    }

	void Metal3ShaderModule::CompileSlang(MTL::Device* device, const std::filesystem::path& path)
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

    	Slang::ComPtr<slang::IModule> module{
    		session->loadModuleFromSource(m_Name.c_str(), path.string().c_str(), nullptr)
    	};
    	Slang::ComPtr<ISlangBlob> msl;
    	module->getTargetCode(0, msl.writeRef());

    	MTL::CompileOptions* compileOptions = MTL::CompileOptions::alloc()->init();
    	compileOptions->setCompileSymbolVisibility(MTL::CompileSymbolVisibilityDefault);
    	NS::Error* error;

    	m_Library = device->newLibrary(NS::String::string(static_cast<const char*>(msl->getBufferPointer()), NS::ASCIIStringEncoding), compileOptions, &error);
		NB_CORE_ASSERT(m_Library, std::format("Could not compile Metal shader {}: {}", m_Name, error->localizedDescription()->utf8String()))

    	GenerateLayout(module);
    }

	void Metal3ShaderModule::GenerateLayout(Slang::ComPtr<slang::IModule> module)
    {
    	slang::ProgramLayout* programLayout = module->getLayout();
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

	uint32_t Metal3ShaderModule::GetUniformBlockLocation(const std::string& uniformBlockName)
	{
    	if (m_Layout.ResourceIndex.find(uniformBlockName) == m_Layout.ResourceIndex.end())
		{
			return 0;
		}
    	ResourceBinding resource = m_Layout.Resources[m_Layout.ResourceIndex[uniformBlockName]];
    	return resource.Binding;
	}

	uint32_t Metal3ShaderModule::GetVertexInputLocation()
	{
    	return m_Layout.VertexBufferLocation;
	}
}
