#pragma once

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "Nebula/Graphics/Texture.h"
#include "Nebula/Renderer/Cameras/SceneCamera.h"
#include "Nebula/Scene/ScriptableEntity.h"

namespace Nebula
{

    struct TagComponent
    {
        std::string Tag;

        TagComponent() = default;
        TagComponent(const TagComponent&) = default;
        TagComponent(const std::string& tag)
            : Tag(tag) {}
    };

    struct TransformComponent
    {
        glm::vec3 Translation{0.0f};
        glm::vec3 Rotation{0.0f};
        glm::vec3 Scale{1.0f};

        TransformComponent() = default;
        TransformComponent(const TransformComponent&) = default;
        TransformComponent(const glm::vec3& translation)
            : Translation(translation) {}
        TransformComponent(const glm::vec3& translation, const glm::vec3& rotation, const glm::vec3& scale)
            : Translation(translation), Rotation(rotation), Scale(scale) {}

        [[nodiscard]] glm::mat4 GetTransform() const
        {
            glm::mat4 rotation = glm::toMat4(glm::quat(Rotation));

            return glm::translate(glm::mat4(1.0f), Translation)
                * rotation
                * glm::scale(glm::mat4(1.0f), Scale);
        }
    };

    struct SpriteRendererComponent
    {
        glm::vec4 Color{1.0f};
        Ref<Graphics::Texture2D> Texture{nullptr};
        float TilingFactor = 1.0f;

        SpriteRendererComponent() = default;
        SpriteRendererComponent(const SpriteRendererComponent&) = default;
        SpriteRendererComponent(const glm::vec4& color)
            : Color(color) {}
    };

    struct CameraComponent
    {
        SceneCamera Camera;
        bool Primary = true;
        bool FixedAspectRatio = false;

        CameraComponent() = default;
        CameraComponent(const CameraComponent&) = default;
    };

    struct NativeScriptComponent
    {
        ScriptableEntity* Instance = nullptr;

        ScriptableEntity* (*InstanciateScript)();
        void (*DestroyScript)(NativeScriptComponent*);

        template<typename T>
        void Bind()
        {
            InstanciateScript = []() { return static_cast<ScriptableEntity*>(new T()); };
            DestroyScript = [](NativeScriptComponent* nsc) { delete nsc->Instance; nsc->Instance = nullptr; };
        }
    };

}
