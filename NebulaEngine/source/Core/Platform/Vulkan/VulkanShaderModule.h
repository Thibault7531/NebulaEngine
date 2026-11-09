#pragma once

#include <slang-com-ptr.h>
#include <slang.h>

#include "VulkanDevice.h"
#include "Nebula/Graphics/ShaderModule.h"
#include "vulkan/vulkan.hpp"

namespace Nebula::Graphics
{
    class VulkanShaderModule : public ShaderModule
    {
    public:
        VulkanShaderModule(Device* device, const std::filesystem::path& filepath);
        ~VulkanShaderModule() override;

        virtual const std::string& GetName() const override { return m_Name; }

    public:
        vk::ShaderModule GetVKShaderModule() { return m_ShaderModule; }
        const ShaderReflectionLayout& GetLayout() const { return m_Layout; }

    private:
        void CompileSlang(const std::filesystem::path& filepath);
        void GenerateLayout(Slang::ComPtr<slang::IModule> module);

    private:
        std::string m_Name;
        VulkanDevice* m_Device;

        vk::ShaderModule m_ShaderModule;

        ShaderReflectionLayout m_Layout;

    private:
        static bool s_IsSlangGlobalSessionCreated;
        static Slang::ComPtr<slang::IGlobalSession> s_GlobalSession;
    };
}
