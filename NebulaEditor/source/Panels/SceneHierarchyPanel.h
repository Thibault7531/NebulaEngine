#pragma once

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Scene/Entity.h"
#include "Nebula/Scene/Scene.h"

namespace Nebula
{

    class SceneHierarchyPanel
    {
    public:
        SceneHierarchyPanel() = default;
        SceneHierarchyPanel(const Ref<Scene>& context);

        void SetContext(const Ref<Scene>& context);
        Entity GetSelectedEntity() const { return m_SelectedEntity; }
        void SetSelectedEntity(Entity entity) { m_SelectedEntity = entity; }

        void OnImGuiRender();

    private:
        void DrawEntityNode(Entity entity);
        void DrawComponents(Entity entity);

    private:
        Ref<Scene> m_Context;

        Entity m_SelectedEntity;

    };

}
