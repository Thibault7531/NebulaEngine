#pragma once

#include <filesystem>
#include <glm/glm.hpp>

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

    class ShaderModule
    {
    public:
        virtual ~ShaderModule() = default;

        [[nodiscard]] virtual const std::string& GetName() const = 0;
    };

    class ShaderModuleLibrary
    {
    public:
        ShaderModuleLibrary() = default;
        ~ShaderModuleLibrary();

        void Add(const std::string& name, const Ref<ShaderModule>& shader);
        void Add(const Ref<ShaderModule>& shader);
        Ref<ShaderModule> Load(const std::filesystem::path& filepath);
        Ref<ShaderModule> Load(const std::string& name, const std::filesystem::path& filepath);
        void Clear();

        Ref<ShaderModule> Get(const std::string& name);

        [[nodiscard]] bool Exists(const std::string& name) const;
    private:
        std::unordered_map<std::string, Ref<ShaderModule>> m_Shaders;
    };
}
