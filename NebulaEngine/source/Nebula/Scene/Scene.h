#pragma once

#include <entt/entt.hpp>

#include "Components.h"
#include "Nebula/Core/Timestep.h"

namespace Nebula
{
    class Entity;

    class Scene
    {
    public:
        Scene();
        ~Scene();

        void OnUpdate(Timestep ts);

        Entity CreateEntity(const std::string& name = "Unnamed Entity");

    private:
        entt::registry m_Registry;

        friend class Entity;

    };

}
