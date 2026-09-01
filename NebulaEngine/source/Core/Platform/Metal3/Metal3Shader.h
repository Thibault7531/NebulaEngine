#pragma once

#include <slang-com-ptr.h>
#include <slang.h>

#include "Metal/Metal.hpp"
#include "Nebula/Graphics/Shader.h"

namespace Nebula::Graphics
{
    struct VertexAttributeInfo
    {
        uint32_t attributeIndex;
        MTL::VertexFormat format;
        uint32_t byteSize;
        uint32_t byteOffset;
    };

    class Metal3Shader : public Shader
    {
    public:
        Metal3Shader(MTL::Device* device, const std::filesystem::path& path);
        ~Metal3Shader();

        virtual const std::string& GetName() const override { return m_Name; }

    private:
        std::string ReadFile(const std::filesystem::path& path);
        void CompileMSL(MTL::Device* device, const std::string& vertexSource, const std::string& fragmentSource);
        void CompileSlang(MTL::Device* device, const std::string& source);
        void GenerateLayout();

    public:
        MTL::Function* GetVertexFunction() { return m_VertexFunction; }
        MTL::Function* GetFragmentFunction() { return m_FragmentFunction; }

        uint32_t GetUniformBlockLocation(const std::string& uniformBlockName);
        uint32_t GetVertexInputLocation();

    private:
        std::string m_Name;

        MTL::Library* m_VertexLibrary;
        MTL::Function* m_VertexFunction;
        MTL::Library* m_FragmentLibrary;
        MTL::Function* m_FragmentFunction;

        // TODO: Implement shader layout caching
        Slang::ComPtr<slang::ISession> m_Session;
        Slang::ComPtr<slang::IComponentType> m_ProgramReflection;
        ShaderReflectionLayout m_Layout;

    private:
        static bool s_IsSlangGlobalSessionCreated;
        static Slang::ComPtr<slang::IGlobalSession> s_GlobalSession;
    };
}
