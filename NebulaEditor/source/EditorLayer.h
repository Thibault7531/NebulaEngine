#pragma once

#include <glm.hpp>

#include "Nebula.h"

namespace Nebula {

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override = default;

        virtual void OnAttach() override;

        void OnUpdate(Timestep ts) override;
        void OnEvent(Event &event) override;
        void OnImGuiRender() override;

    private:
        bool OnKeyPressedEvent(const KeyPressedEvent& event);

    private:
        bool IsViewportFocused = false;
        bool IsViewportHovered = false;
        glm::vec2 m_ViewportSize = { 1920.0f, 1080.0f };
        Ref<Framebuffer> m_Framebuffer;

        OrthographicCameraController m_Camera;

        Ref<Scene> m_Scene;
        Entity m_SquareEntity;

    };

}
