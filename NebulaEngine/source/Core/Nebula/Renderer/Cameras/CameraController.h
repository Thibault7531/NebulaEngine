#pragma once

#include "OrthographicCamera.h"
#include "PerspectiveCamera.h"
#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Platform/Nebula/CoreSystems/Events/Event.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"


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
        void OnEvent(Platform::Event& e);

    private:
        bool OnMouseScrolled(const Platform::MouseScrolledEvent& e);
        bool OnWindowResize(const Platform::WindowResizedEvent& e);

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
        void OnEvent(Platform::Event& e);

    private:
        bool OnMouseMoveEvent(const Platform::MouseMovedEvent& e);
        bool OnWindowResize(const Platform::WindowResizedEvent& e);

    private:
        float m_aspectRatio = 16.0f/9.0f;
        float m_FOV = 60.0f;
        PerspectiveCamera m_Camera;

        float m_CameraTranslationSpeed = 1.0f;

        glm::vec2 m_PreviousMousePosition = { 0.0f, 0.0f };
        Timestep m_LastFrameTime = 0.0f;
    };

}
