#pragma once

#include <glm.hpp>
#include <imgui.h>
#include <ImGuizmo.h>

#include "Nebula.h"
#include "Nebula/CoreSystems/Application/Layer.h"
#include "Nebula/Graphics/Buffers/Framebuffer.h"
#include "Nebula/Renderer/Cameras/EditorCamera.h"
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
        bool OnMouseButtonPressedEvent(const Platform::MouseButtonPressedEvent& event);

    private:
        bool m_MouseCursorChanged = false;

        bool IsViewportFocused = false;
        bool IsViewportHovered = false;
        bool DoesViewportFit = false;
        glm::vec2 m_ViewportPos = { 0.0f, 0.0f };
        glm::vec2 m_ViewportSize = { 1920.0f, 1080.0f };
        Ref<Graphics::Device> m_Device;
        Ref<Graphics::Framebuffer> m_Framebuffer;

        EditorCamera m_Camera;

        Ref<Scene> m_Scene;
        int m_GizmoOperation = -1;

        // Panels
        SceneHierarchyPanel m_SceneHierarchyPanel;

        bool IsFirstRender = true;
        std::vector<glm::vec4> m_TitleBarButtons{};

        Ref<Graphics::Texture2D> m_CloseIcon;
        Ref<Graphics::Texture2D> m_MaximizeIcon;
        Ref<Graphics::Texture2D> m_RestoreIcon;
        Ref<Graphics::Texture2D> m_MinimizeIcon;
    };

}
