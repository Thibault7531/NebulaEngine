#pragma once

#include <filesystem>
#include <glm/glm.hpp>

#include "ShaderPipeline.h"
#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula::Graphics
{
    enum class ResourceType
    {
        UniformBuffer, StorageBuffer, Texture, Sampler, PushConstant
    };
    enum class ShaderStage : uint8_t
    {
        Vertex = 1 << 0, Fragment = 1 << 1, Compute = 1 << 2
    };

    struct ResourceBinding
    {
        std::string Name;
        ResourceType Type;
        uint32_t Set;
        uint32_t Binding;
        uint32_t Size;
        uint8_t StageFlags;
    };

    struct ShaderReflectionLayout
    {
        std::vector<ResourceBinding> Resources;
        std::unordered_map<std::string, uint32_t> ResourceIndex;
        uint32_t VertexBufferLocation;
    };

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
        ~ShaderLibrary();

        void Add(const std::string& name, const Ref<Shader>& shader);
        void Add(const Ref<Shader>& shader);
        Ref<Shader> Load(const std::filesystem::path& filepath);
        Ref<Shader> Load(const std::string& name, const std::filesystem::path& filepath);
        void Clear();

        Ref<Shader> Get(const std::string& name);

        [[nodiscard]] bool Exists(const std::string& name) const;

        Ref<ShaderPipeline> CreatePipeline(const std::string& shaderName, const VertexLayout& layout, const PipelineSpecification& spec);
    private:
        std::unordered_map<std::string, Ref<Shader>> m_Shaders;
    };
}
