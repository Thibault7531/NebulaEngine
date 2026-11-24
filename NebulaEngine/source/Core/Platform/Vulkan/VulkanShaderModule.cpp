#include "VulkanShaderModule.h"

#include <limits>

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
        m_Layout = {};
        slang::ProgramLayout* programLayout = module->getLayout();
        const uint32_t paramCount = programLayout->getParameterCount();

        for (uint32_t i = 0; i < paramCount; i++)
        {
            slang::VariableLayoutReflection* variableLayout = programLayout->getParameterByIndex(i);
            slang::TypeLayoutReflection* typeLayout = variableLayout->getTypeLayout();
            if (!typeLayout || !variableLayout->getVariable())
            {
                continue;
            }

            uint32_t descriptorCount = 1;
            while (typeLayout->isArray())
            {
                const size_t elementCount = typeLayout->getElementCount(programLayout);
                if (elementCount != SLANG_UNBOUNDED_SIZE && elementCount != SLANG_UNKNOWN_SIZE &&
                    elementCount > 0 && elementCount <= std::numeric_limits<uint32_t>::max())
                {
                    const uint32_t count = static_cast<uint32_t>(elementCount);
                    if (descriptorCount > std::numeric_limits<uint32_t>::max() / count)
                    {
                        descriptorCount = std::numeric_limits<uint32_t>::max();
                    }
                    else
                    {
                        descriptorCount *= count;
                    }
                }
                typeLayout = typeLayout->getElementTypeLayout();
            }

            ResourceType resourceType;
            switch (typeLayout->getKind())
            {
            case slang::TypeReflection::Kind::ConstantBuffer:
            case slang::TypeReflection::Kind::ParameterBlock:
                resourceType = ResourceType::UniformBuffer;
                break;
            case slang::TypeReflection::Kind::ShaderStorageBuffer:
                resourceType = ResourceType::StorageBuffer;
                break;
            case slang::TypeReflection::Kind::Resource:
            case slang::TypeReflection::Kind::TextureBuffer:
                resourceType = ResourceType::Texture;
                break;
            case slang::TypeReflection::Kind::SamplerState:
                resourceType = ResourceType::Sampler;
                break;
            default:
                continue;
            }

            const auto bindingCategory = slang::ParameterCategory::DescriptorTableSlot;
            const size_t binding = variableLayout->getOffset(bindingCategory);
            const size_t set = variableLayout->getBindingSpace(bindingCategory);
            if (binding == SLANG_UNKNOWN_SIZE || binding == SLANG_UNBOUNDED_SIZE ||
                set == SLANG_UNKNOWN_SIZE || set == SLANG_UNBOUNDED_SIZE ||
                binding > std::numeric_limits<uint32_t>::max() ||
                set > std::numeric_limits<uint32_t>::max())
            {
                NB_CORE_ERROR("Vulkan shader resource '{0}' has an unresolved binding",
                    variableLayout->getVariable()->getName());
                continue;
            }

            ResourceBinding resource;
            resource.Name = variableLayout->getVariable()->getName();
            resource.Type = resourceType;
            resource.Set = static_cast<uint32_t>(set);
            resource.Binding = static_cast<uint32_t>(binding);
            const size_t uniformSize = typeLayout->getSize(
                resourceType == ResourceType::UniformBuffer
                    ? slang::ParameterCategory::ConstantBuffer
                    : slang::ParameterCategory::Uniform);
            const size_t uniformOffset = variableLayout->getOffset(
                resourceType == ResourceType::UniformBuffer
                    ? slang::ParameterCategory::ConstantBuffer
                    : slang::ParameterCategory::Uniform);
            resource.Size = uniformSize == SLANG_UNKNOWN_SIZE || uniformSize == SLANG_UNBOUNDED_SIZE
                ? 0
                : static_cast<uint32_t>(std::min(uniformSize,
                    static_cast<size_t>(std::numeric_limits<uint32_t>::max())));
            resource.Offset = uniformOffset == SLANG_UNKNOWN_SIZE || uniformOffset == SLANG_UNBOUNDED_SIZE
                ? 0
                : static_cast<uint32_t>(std::min(uniformOffset,
                    static_cast<size_t>(std::numeric_limits<uint32_t>::max())));
            resource.DescriptorCount = descriptorCount;

            if (m_Layout.ResourceIndex.contains(resource.Name))
            {
                NB_CORE_ERROR("Duplicate Vulkan shader resource name '{0}'", resource.Name);
                continue;
            }

            m_Layout.ResourceIndex[resource.Name] = static_cast<uint32_t>(m_Layout.Resources.size());
            m_Layout.Resources.push_back(resource);
        }

        uint32_t entrypointCount = programLayout->getEntryPointCount();
        for (uint32_t i = 0; i < entrypointCount; i++)
        {
            slang::EntryPointLayout* entrypointLayout = programLayout->getEntryPointByIndex(i);
            if (entrypointLayout->getStage() != SLANG_STAGE_FRAGMENT)
            {
                continue;
            }

            slang::VariableLayoutReflection* resultVar = entrypointLayout->getResultVarLayout();
            if (resultVar)
            {
                slang::TypeLayoutReflection* resultTypeLayout = resultVar->getTypeLayout();
                slang::TypeReflection::Kind resultKind = resultTypeLayout->getKind();

                uint32_t location = resultVar->getOffset(slang::ParameterCategory::VaryingOutput);

                if (resultKind == slang::TypeReflection::Kind::Struct)
                {
                    uint32_t fieldCount = resultTypeLayout->getFieldCount();
                    for (uint32_t j = 0; j < fieldCount; j++)
                    {
                        slang::VariableLayoutReflection* fieldVar = resultTypeLayout->getFieldByIndex(j);
                        slang::TypeLayoutReflection* fieldTypeLayout = fieldVar->getTypeLayout();
                        slang::TypeReflection::Kind fieldKind = fieldTypeLayout->getKind();
                        if (fieldKind == slang::TypeReflection::Kind::Resource)
                        {
                            if (fieldVar->getSemanticName() && std::string(fieldVar->getSemanticName()) == "SV_Target")
                            {
                                m_Layout.colorAttachmentCount++;
                            }
                        }
                    }
                }
                else if (resultKind == slang::TypeReflection::Kind::Resource)
                {
                    if (resultVar->getSemanticName() && std::string(resultVar->getSemanticName()) == "SV_Target")
                    {
                        m_Layout.colorAttachmentCount++;
                    }
                }
            }
        }
    }
}
