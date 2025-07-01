#include "SceneSerializer.h"

#include <filesystem>
#include <yaml-cpp/yaml.h>

#include "Components.h"
#include "Entity.h"
#include "yaml-cpp/emittermanip.h"

namespace YAML
{

    template<>
    struct convert<glm::vec3>
    {
        static Node encode(const glm::vec3& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            return node;
        }

        static bool decode(const Node& node, glm::vec3& rhs)
        {
            if (!node.IsSequence() || node.size() != 3)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            return true;
        }
    };

    template<>
    struct convert<glm::vec4>
    {
        static Node encode(const glm::vec4& rhs)
        {
            Node node;
            node.push_back(rhs.x);
            node.push_back(rhs.y);
            node.push_back(rhs.z);
            node.push_back(rhs.w);
            return node;
        }

        static bool decode(const Node& node, glm::vec4& rhs)
        {
            if (!node.IsSequence() || node.size() != 4)
                return false;

            rhs.x = node[0].as<float>();
            rhs.y = node[1].as<float>();
            rhs.z = node[2].as<float>();
            rhs.w = node[3].as<float>();
            return true;
        }
    };

}

namespace Nebula
{

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec3& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
        return out;
    }

    YAML::Emitter& operator<<(YAML::Emitter& out, const glm::vec4& v)
    {
        out << YAML::Flow;
        out << YAML::BeginSeq << v.x << v.y << v.z << v.w << YAML::EndSeq;
        return out;
    }

    SceneSerializer::SceneSerializer(const Ref<Scene>& scene)
        : m_Scene(scene)
    {
    }

    void SceneSerializer::Serialize(const std::string& filepath)
    {
        YAML::Emitter out;
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Scene" << YAML::Value << "Unnamed Scene";

            out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
            for (auto entityID : m_Scene->m_Registry.view<entt::entity>())
            {
                Entity entity{ entityID, m_Scene.get() };
                if (!entity) continue;

                SerializeEntity(out, entity);
            }
            out << YAML::EndSeq;
        }
        out << YAML::EndMap;

        std::filesystem::create_directories(filepath.substr(0, filepath.find_last_of("/\\") + 1));
        std::ofstream fout(filepath);
        NB_CORE_ASSERT(fout.is_open(), "Failed to open file!");
        fout << out.c_str();
    }

    void SceneSerializer::SerializeRuntime(const std::string& filepath)
    {
        // Not implemented
        NB_CORE_ASSERT(false);
    }

    bool SceneSerializer::Deserialize(const std::string& filepath)
    {
        std::ifstream stream(filepath);
        std::stringstream strStream;
        strStream << stream.rdbuf();

        YAML::Node data = YAML::Load(strStream.str());
        if (!data["Scene"]) return false;

        std::string sceneName = data["Scene"].as<std::string>();
        NB_CORE_TRACE("Deserializing scene \"{}\"", sceneName.c_str());

        auto entities = data["Entities"];
        if (entities)
        {
            for (auto entity : entities)
            {
                uint64_t uuid = entity["Entity"].as<uint64_t>();

                std::string tag;
                auto tagComponent = entity["TagComponent"];
                if (tagComponent) tag = tagComponent["Tag"].as<std::string>();

                NB_CORE_TRACE("Deserializing entity with ID = {}, name = {}", uuid, tag);

                Entity deserializedEntity = m_Scene->CreateEntity(tag);

                auto transformComponent = entity["TransformComponent"];
                if (transformComponent)
                {
                    auto& tc = deserializedEntity.GetComponent<TransformComponent>();
                    tc.Translation = transformComponent["Translation"].as<glm::vec3>();
                    tc.Rotation = transformComponent["Rotation"].as<glm::vec3>();
                    tc.Scale = transformComponent["Scale"].as<glm::vec3>();
                }

                auto spriteRendererComponent = entity["SpriteRendererComponent"];
                if (spriteRendererComponent)
                {
                    deserializedEntity.AddComponent<SpriteRendererComponent>(spriteRendererComponent["Color"].as<glm::vec4>());
                }

                auto cameraComponent = entity["CameraComponent"];
                if (cameraComponent)
                {
                    auto& cc = deserializedEntity.AddComponent<CameraComponent>();
                    auto& camera = cc.Camera;

                    camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(cameraComponent["Camera"]["ProjectionType"].as<int>()));
                    camera.SetPerspectiveVerticalFOV(cameraComponent["Camera"]["PerspectiveFOV"].as<float>());
                    camera.SetPerspectiveNearClip(cameraComponent["Camera"]["PerspectiveNear"].as<float>());
                    camera.SetPerspectiveFarClip(cameraComponent["Camera"]["PerspectiveFar"].as<float>());
                    camera.SetOrthographicSize(cameraComponent["Camera"]["OrthographicSize"].as<float>());
                    camera.SetOrthographicNearClip(cameraComponent["Camera"]["OrthographicNear"].as<float>());
                    camera.SetOrthographicFarClip(cameraComponent["Camera"]["OrthographicFar"].as<float>());

                    cc.Primary = cameraComponent["Primary"].as<bool>();
                    cc.FixedAspectRatio = cameraComponent["FixedAspectRatio"].as<bool>();
                }
            }
        }

        return true;
    }

    bool SceneSerializer::DeserializeRuntime(const std::string& filepath)
    {
        // Not implemented
        NB_CORE_ASSERT(false);
        return false;
    }

    void SceneSerializer::SerializeEntity(YAML::Emitter& out, Entity entity)
    {
        out << YAML::BeginMap;
        {
            out << YAML::Key << "Entity" << YAML::Value << "NOID";

            if (entity.HasComponent<TagComponent>())
            {
                out << YAML::Key << "TagComponent" << YAML::BeginMap;
                {
                    auto& tag = entity.GetComponent<TagComponent>().Tag;
                    out << YAML::Key << "Tag" << YAML::Value << tag;
                }
                out << YAML::EndMap;
            }

            if (entity.HasComponent<TransformComponent>())
            {
                out << YAML::Key << "TransformComponent" << YAML::BeginMap;
                {
                    auto& tc = entity.GetComponent<TransformComponent>();
                    out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
                    out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
                    out << YAML::Key << "Scale" << YAML::Value << tc.Scale;
                }
                out << YAML::EndMap;
            }

            if (entity.HasComponent<SpriteRendererComponent>())
            {
                out << YAML::Key << "SpriteRendererComponent" << YAML::BeginMap;
                {
                    auto& src = entity.GetComponent<SpriteRendererComponent>();
                    out << YAML::Key << "Color" << YAML::Value << src.Color;
                }
                out << YAML::EndMap;
            }

            if (entity.HasComponent<CameraComponent>())
            {
                out << YAML::Key << "CameraComponent" << YAML::BeginMap;
                {
                    auto& cc = entity.GetComponent<CameraComponent>();
                    auto& camera = cc.Camera;

                    out << YAML::Key << "Camera" << YAML::Value << YAML::BeginMap;
                    {
                        out << YAML::Key << "ProjectionType" << YAML::Value << static_cast<int>(camera.GetProjectionType());
                        out << YAML::Key << "PerspectiveFOV" << YAML::Value << camera.GetPerspectiveVerticalFOV();
                        out << YAML::Key << "PerspectiveNear" << YAML::Value << camera.GetPerspectiveNearClip();
                        out << YAML::Key << "PerspectiveFar" << YAML::Value << camera.GetPerspectiveFarClip();
                        out << YAML::Key << "OrthographicSize" << YAML::Value << camera.GetOrthographicSize();
                        out << YAML::Key << "OrthographicNear" << YAML::Value << camera.GetOrthographicNearClip();
                        out << YAML::Key << "OrthographicFar" << YAML::Value << camera.GetOrthographicFarClip();
                    }
                    out << YAML::EndMap;

                    out << YAML::Key << "Primary" << YAML::Value << cc.Primary;
                    out << YAML::Key << "FixedAspectRatio" << YAML::Value << cc.FixedAspectRatio;
                }
                out << YAML::EndMap;
            }
        }

        out << YAML::EndMap;
    }

}
