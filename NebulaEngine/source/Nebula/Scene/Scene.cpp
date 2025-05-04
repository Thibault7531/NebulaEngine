#include "Scene.h"

#include <glm/glm.hpp>

#include "Entity.h"
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
        auto group = m_Registry.group<TransformComponent>(entt::get<SpriteRendererComponent>);
        for (auto entity : group)
        {
            auto [transform, sprite] = group.get<TransformComponent, SpriteRendererComponent>(entity);

            Renderer2D::DrawQuad({.transform = transform, .color = sprite.Color});
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
