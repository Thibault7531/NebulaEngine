#include "VulkanShaderModule.h"

namespace Nebula::Graphics
{
    bool VulkanShaderModule::s_IsSlangGlobalSessionCreated = false;
    Slang::ComPtr<slang::IGlobalSession> VulkanShaderModule::s_GlobalSession = nullptr;

    VulkanShaderModule::VulkanShaderModule(Device* device, const std::filesystem::path& filepath)
        : m_Device{dynamic_cast<VulkanDevice*>(device)}
    {
        m_Name = filepath.stem().string();
        CompileSlang(filepath);
    }

    VulkanShaderModule::~VulkanShaderModule()
    {
        m_Device->GetVKLogicalDevice().destroyShaderModule(m_ShaderModule);
    }

    void VulkanShaderModule::CompileSlang(const std::filesystem::path& filepath)
    {
        if (!s_IsSlangGlobalSessionCreated)
        {
            slang::createGlobalSession(s_GlobalSession.writeRef());
            s_IsSlangGlobalSessionCreated = true;
        }

        auto slangTargets{ std::to_array<slang::TargetDesc>({
            {
                .format{SLANG_SPIRV},
                .profile{s_GlobalSession->findProfile("spirv_1_4")}
            } })};
        auto slangOptions{ std::to_array<slang::CompilerOptionEntry>({
            {
                slang::CompilerOptionName::EmitSpirvDirectly,
                { slang::CompilerOptionValueKind::Int, 1}
            }})};
        slang::SessionDesc sessionDesc;
        sessionDesc.targets = slangTargets.data();
        sessionDesc.targetCount = slangTargets.size();
        sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_ROW_MAJOR;
        sessionDesc.compilerOptionEntries = slangOptions.data();
        sessionDesc.compilerOptionEntryCount = slangOptions.size();
        Slang::ComPtr<slang::ISession> session;
        s_GlobalSession->createSession(sessionDesc, session.writeRef());

        Slang::ComPtr<slang::IModule> module{
            session->loadModuleFromSource(m_Name.c_str(), filepath.string().c_str(), nullptr)
        };
        Slang::ComPtr<ISlangBlob> spirv;
        module->getTargetCode(0, spirv.writeRef());

        vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
        shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        shaderModuleCreateInfo.codeSize = spirv->getBufferSize();
        shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(spirv->getBufferPointer());
        m_ShaderModule = m_Device->GetVKLogicalDevice().createShaderModule(shaderModuleCreateInfo);

        GenerateLayout(module);
    }

    void VulkanShaderModule::GenerateLayout(Slang::ComPtr<slang::IModule> module)
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
            res.Size = typeLayout->getSize(slang::ParameterCategory::ConstantBuffer);
            res.Offset = varLayout->getOffset(slang::ParameterCategory::ConstantBuffer);

            switch (typeLayout->getKind())
            {
            case slang::TypeReflection::Kind::ConstantBuffer:
            case slang::TypeReflection::Kind::ParameterBlock:
                {
                    res.Type = ResourceType::UniformBuffer;
                    res.Binding = varLayout->getOffset(slang::ParameterCategory::DescriptorTableSlot);
                    res.Set = varLayout->getBindingSpace(slang::ParameterCategory::DescriptorTableSlot);
                    maxBinding = std::max(res.Binding, maxBinding);
                    break;
                }

            case slang::TypeReflection::Kind::Resource:
                {
                    res.Type = ResourceType::Texture;
                    res.Binding = varLayout->getOffset(slang::ParameterCategory::DescriptorTableSlot);
                    res.Set = varLayout->getBindingSpace(slang::ParameterCategory::DescriptorTableSlot);
                    maxBinding = std::max(res.Binding, maxBinding);
                    break;
                }

            case slang::TypeReflection::Kind::SamplerState:
                {
                    res.Type = ResourceType::Sampler;
                    res.Binding = varLayout->getOffset(slang::ParameterCategory::DescriptorTableSlot);
                    res.Set = varLayout->getBindingSpace(slang::ParameterCategory::DescriptorTableSlot);
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
}
