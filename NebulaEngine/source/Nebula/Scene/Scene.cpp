#include "Scene.h"

#include <glm/glm.hpp>

#include "Components.h"
#include "Nebula/Renderer/Renderer2D.h"

namespace Nebula
{

    Scene::Scene()
    {
        const entt::entity entity = m_Registry.create();
        m_Registry.emplace<TransformComponent>(entity, glm::mat4(1.0f));
    }

    Scene::~Scene()
    {
        m_Registry.clear();
    }

    void Scene::OnUpdate(Timestep ts)
    {
        {
            m_Registry.view<NativeScriptComponent>().each([=](auto entity, auto& nsc)
            {
                if (!nsc.Instance)
                {
                    nsc.Instance = nsc.InstanciateScript();
                    nsc.Instance->m_Entity = Entity{ entity, this };
                    nsc.Instance->OnCreate();
                }

                nsc.Instance->OnUpdate(ts);
            });
        }

        Camera* primaryCamera = nullptr;
        glm::mat4 cameraTransform;
        {
            auto view = m_Registry.view<TransformComponent, CameraComponent>();
            for (auto entity : view)
            {
                auto [transform, camera] = view.get<TransformComponent, CameraComponent>(entity);

                if (camera.Primary)
                {
                    primaryCamera = &camera.Camera;
                    cameraTransform = transform.Transform;
                    break;
                }
            }
        }

        if (primaryCamera)
        {
            Renderer2D::BeginScene(*primaryCamera, cameraTransform);

            for (const auto view = m_Registry.view<TransformComponent, SpriteRendererComponent>(); const auto entity : view)
            {
                auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

                Renderer2D::DrawQuad({.transform = transform, .color = sprite.Color});
            }

            Renderer2D::EndScene();
        }
    }

    void Scene::OnViewportResize(uint32_t width, uint32_t height)
    {
        m_viewportWidth = width;
        m_viewportHeight = height;

        auto view = m_Registry.view<CameraComponent>();
        for (auto entity : view)
        {
            auto& cameraComponent = view.get<CameraComponent>(entity);
            if (!cameraComponent.FixedAspectRatio)
            {
                cameraComponent.Camera.SetViewportSize(width, height);
            }
        }
    }

    Entity Scene::CreateEntity(const std::string& name)
    {
        Entity entity{m_Registry.create(), this};
        entity.AddComponent<TagComponent>(name);
        entity.AddComponent<TransformComponent>();
        return entity;
    }

}
