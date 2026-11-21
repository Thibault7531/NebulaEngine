#pragma once

#include <slang-com-ptr.h>
#include <slang.h>

#include "Metal/Metal.hpp"
#include "Nebula/Graphics/ShaderModule.h"

namespace Nebula::Graphics
{
    struct VertexAttributeInfo
    {
        uint32_t attributeIndex;
        MTL::VertexFormat format;
        uint32_t byteSize;
        uint32_t byteOffset;
    };

    class Metal3ShaderModule : public ShaderModule
    {
    public:
        Metal3ShaderModule(MTL::Device* device, const std::filesystem::path& path);
        ~Metal3ShaderModule();

        virtual const std::string& GetName() const override { return m_Name; }

    private:
        void CompileSlang(MTL::Device* device, const std::filesystem::path& path);
        void GenerateLayout(Slang::ComPtr<slang::IModule> module);

    public:
        MTL::Library* GetMTLLibrary() { return m_Library; }
        const ShaderReflectionLayout& GetLayout() const { return m_Layout; }

        uint32_t GetUniformBlockLocation(const std::string& uniformBlockName);
        uint32_t GetVertexInputLocation();

    private:
        std::string m_Name;

        MTL::Library* m_Library;

        ShaderReflectionLayout m_Layout;

    private:
        static bool s_IsSlangGlobalSessionCreated;
        static Slang::ComPtr<slang::IGlobalSession> s_GlobalSession;
    };
}
