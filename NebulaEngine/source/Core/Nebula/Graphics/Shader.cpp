#include "Shader.h"

#include "Nebula/CoreSystems/Application/Application.h"

namespace Nebula::Graphics
{
    ShaderLibrary::~ShaderLibrary()
    {
        m_Shaders.clear();
    }

    void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
    {
        m_Shaders[name] = shader;
    }

    void ShaderLibrary::Add(const Ref<Shader>& shader)
    {
        m_Shaders[shader->GetName()] = shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::filesystem::path& filepath)
    {
        Ref<Shader> shader = Application::Get().GetDevice()->CreateShader(filepath);
        Add(shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
    {
        Ref<Shader> shader = Application::Get().GetDevice()->CreateShader(filepath);
        Add(name, shader);
        return shader;
    }

    Ref<Shader> ShaderLibrary::Get(const std::string& name)
    {
        return m_Shaders[name];
    }

    bool ShaderLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.contains(name);
    }

    Ref<ShaderPipeline> ShaderLibrary::CreatePipeline(const std::string& shaderName, const VertexLayout& layout, const PipelineSpecification& spec)
    {
        return Application::Get().GetDevice()->CreateShaderPipeline(m_Shaders[shaderName], layout, spec);
    }
}
