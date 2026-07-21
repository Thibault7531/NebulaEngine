#pragma once

#include <glm/glm.hpp>

#include "ShaderPipeline.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    class Shader
    {
    public:
        virtual ~Shader() = default;

        [[nodiscard]] virtual const std::string& GetName() const = 0;
    };

    class ShaderLibrary
    {
    public:
        ShaderLibrary() = default;

        void Add(const std::string& name, const Ref<Shader>& shader);
        void Add(const Ref<Shader>& shader);
        Ref<Shader> Load(const std::filesystem::path& filepath);
        Ref<Shader> Load(const std::string& name, const std::filesystem::path& filepath);

        Ref<Shader> Get(const std::string& name);

        [[nodiscard]] bool Exists(const std::string& name) const;

        Ref<ShaderPipeline> CreatePipeline(const std::string& shaderName, const VertexLayout& layout, const PipelineSpecification& spec);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}
