#pragma once

#include <entt/entt.hpp>

#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Nebula/Renderer/Cameras/EditorCamera.h"

namespace Nebula
{
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void OnUpdateRuntime(Timestep ts);
        void OnViewportResize(uint32_t width, uint32_t height);

        Entity CreateEntity(const std::string& name = "Unnamed Entity");
        void DestroyEntity(Entity entity);

        Entity GetPrimaryCameraEntity();
        entt::registry& GetRegistry() { return m_Registry; }

    private:
        template<typename T>
        void OnComponentAdded(Entity entity, T& component);

    private:
        entt::registry m_Registry;

        uint32_t m_viewportWidth = 0, m_viewportHeight = 0;

        friend class Entity;
        friend class SceneSerializer;
        friend class SceneHierarchyPanel;

    };

}
