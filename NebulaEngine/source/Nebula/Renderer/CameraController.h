#pragma once

#include "../Core/Timestep.h"
#include "../Events/ApplicationEvent.h"
#include "../Events/Event.h"
#include "../Events/MouseEvent.h"
#include "Camera.h"

namespace Nebula
{

    class OrthographicCameraController
    {
    public:
        OrthographicCameraController(float aspectRatio, bool rotation = false);

        void OnResize(float width, float height);

        OrthographicCamera& GetCamera() { return m_Camera; }
        const OrthographicCamera& GetCamera() const { return m_Camera; }

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

    private:
        bool OnMouseScrolled(const MouseScrolledEvent& e);
        bool OnWindowResize(const WindowResizeEvent& e);

    private:
        float m_aspectRatio;
        float m_zoomLevel = 1.0f;
        OrthographicCamera m_Camera;

        bool m_Rotation = false;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        float m_CameraRotation = 0.0f;
        float m_CameraTranslationSpeed = 1.0f, m_CameraRotationSpeed = 90.0f;
    };

    class PerspectiveCameraController
    {
    public:
        PerspectiveCameraController(float aspectRatio, float FOV);

        PerspectiveCamera& GetCamera() { return m_Camera; }
        const PerspectiveCamera& GetCamera() const { return m_Camera; }

        void OnUpdate(Timestep ts);
        void OnEvent(Event& e);

    private:
        bool OnMouseMoveEvent(const MouseMovedEvent& e);
        bool OnWindowResize(const WindowResizeEvent& e);

    private:
        float m_aspectRatio = 16.0f/9.0f;
        float m_FOV = 60.0f;
        PerspectiveCamera m_Camera;

        glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_cameraRotation = { 0.0f, -90.0f, 0.0f };
        float m_CameraTranslationSpeed = 1.0f;

        glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };
    };

}
