#pragma once

#include "Scene.h"
#include "yaml-cpp/emitter.h"

namespace Nebula
{

    class SceneSerializer
    {
    public:
        SceneSerializer(const Ref<Scene>& scene);

        void Serialize(const std::string& filepath);
        void SerializeRuntime(const std::string& filepath);

        bool Deserialize(const std::string& filepath);
        bool DeserializeRuntime(const std::string& filepath);

    public:
        static void SerializeEntity(YAML::Emitter& out, Entity entity);

    private:
        Ref<Scene> m_Scene;
    };

}
