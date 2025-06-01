#pragma once

#include <entt/entt.hpp>

#include "Scene.h"
#include "Nebula/Core/Core.h"

namespace Nebula {

    class Entity {
    public:
        Entity() = default;
        Entity(entt::entity handle, Scene* scene);
        Entity(const Entity& other) = default;

        template<typename T, typename... Args>
        T& AddComponent(Args&&... args)
        {
            NB_CORE_ASSERT(!HasComponent<T>(), "Entity already has component")
            NB_CORE_ASSERT(this, "Entity null")

            return m_Scene->m_Registry.emplace<T>(m_EntityHandle, std::forward<Args>(args)...);
        }
        template<typename T>
        bool HasComponent()
        {
            return m_Scene->m_Registry.any_of<T>(m_EntityHandle);
        }
        template<typename T>
        T& GetComponent()
        {
            NB_CORE_ASSERT(HasComponent<T>(), "Entity does not have component")

            return m_Scene->m_Registry.get<T>(m_EntityHandle);
        }
        template<typename T>
        void RemoveComponent()
        {
            NB_CORE_ASSERT(HasComponent<T>(), "Entity does not have component")

            m_Scene->m_Registry.remove<T>(m_EntityHandle);
        }

        operator bool() const { return m_EntityHandle != entt::null; }
        operator uint32_t() const { return static_cast<uint32_t>(m_EntityHandle); }
        bool operator ==(const Entity& other) const { return m_EntityHandle == other.m_EntityHandle && m_Scene == other.m_Scene; }
        bool operator !=(const Entity& other) const { return !operator==(other); }

    private:
        entt::entity m_EntityHandle{ entt::null };
        Scene* m_Scene = nullptr;

    };

}
