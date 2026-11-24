#include "ShaderModule.h"

#include "Nebula/CoreSystems/Application/Application.h"

namespace Nebula::Graphics
{
    ShaderModuleLibrary::~ShaderModuleLibrary()
    {
        m_Shaders.clear();
    }

    void ShaderModuleLibrary::Add(const std::string& name, const Ref<ShaderModule>& shader)
    {
        m_Shaders[name] = shader;
    }

    void ShaderModuleLibrary::Add(const Ref<ShaderModule>& shader)
    {
        m_Shaders[shader->GetName()] = shader;
    }

    Ref<ShaderModule> ShaderModuleLibrary::Load(const std::filesystem::path& filepath)
    {
        Ref<ShaderModule> shader = Application::Get().GetDevice()->CreateShaderModule(filepath);
        Add(shader);
        return shader;
    }

    Ref<ShaderModule> ShaderModuleLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
    {
        Ref<ShaderModule> shader = Application::Get().GetDevice()->CreateShaderModule(filepath);
        Add(name, shader);
        return shader;
    }

    void ShaderModuleLibrary::Clear()
    {
        m_Shaders.clear();
    }

    Ref<ShaderModule> ShaderModuleLibrary::Get(const std::string& name)
    {
        return m_Shaders[name];
    }

    bool ShaderModuleLibrary::Exists(const std::string& name) const
    {
        return m_Shaders.contains(name);
    }
}
