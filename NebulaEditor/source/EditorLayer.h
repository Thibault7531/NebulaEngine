#pragma once
#include "glm/vec2.hpp"
#include "Nebula/Core/Layer.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Renderer/CameraController.h"
#include "Nebula/Renderer/Framebuffer.h"

namespace Nebula {

    class EditorLayer : public Layer {
    public:
        EditorLayer();
        ~EditorLayer() override = default;

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

    };

}
