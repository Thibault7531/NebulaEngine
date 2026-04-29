#pragma once

#include <glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include "Nebula.h"
#include "Nebula/CoreSystems/Application/Layer.h"
#include "Nebula/Renderer/Buffers/Framebuffer.h"
#include "Panels/SceneHierarchyPanel.h"

namespace Nebula {

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override = default;

        virtual void OnAttach() override;
        virtual void OnDetach() override;

        virtual void OnUpdate(Timestep ts) override;
        virtual void OnEvent(Platform::Event &event) override;
        virtual void OnImGuiRender() override;

        void NewScene();
        void OpenScene();
        void SaveSceneAs();

    private:
        bool OnKeyPressedEvent(const Platform::KeyPressedEvent& event);

    private:
        bool m_MouseCursorChanged = false;

        bool IsViewportFocused = false;
        bool IsViewportHovered = false;
        bool DoesViewportFit = false;
        glm::vec2 m_ViewportSize = { 1920.0f, 1080.0f };
        Ref<Framebuffer> m_Framebuffer;

        Ref<Scene> m_Scene;
        int m_GizmoOperation = -1;

        // Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;

        bool IsFirstRender = true;
        std::vector<glm::vec4> m_TitleBarButtons{};

        Ref<Texture2D> m_CloseIcon;
        Ref<Texture2D> m_MaximizeIcon;
        Ref<Texture2D> m_RestoreIcon;
        Ref<Texture2D> m_MinimizeIcon;
    };

}
