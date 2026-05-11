#pragma once

#include "Camera.h"
#include "Nebula/CoreSystems/Core/Timestep.h"
#include "Platform/Nebula/CoreSystems/Events/Event.h"
#include "Platform/Nebula/CoreSystems/Events/MouseEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"

namespace Nebula
{
    class EditorCamera : public Camera
    {
    public:
        EditorCamera() = default;
        EditorCamera(float FOV, float aspectRatio, float nearClip, float farClip);
        virtual ~EditorCamera() = default;

        void OnEvent(Platform::Event& e);

        float GetDistance() const { return m_Distance; }
        void SetDistance(float distance) { m_Distance = distance; }

        void SetViewportSize(uint32_t width, uint32_t height);

        const glm::mat4& GetViewMatrix() const { return m_ViewMatrix; }
        glm::mat4 GetViewProjectionMatrix() const { return m_ProjectionMatrix * m_ViewMatrix; }

        glm::vec3 GetUpVector() const;
        glm::vec3 GetRightVector() const;
        glm::vec3 GetForwardVector() const;
        const glm::vec3& GetPosition() const { return m_Position; }
        glm::quat GetOrientation() const;

        float GetPitch() const { return m_Pitch; }
        void SetPitch(float pitch) { m_Pitch = pitch; RecalculateViewMatrix(); }
        float GetYaw() const { return m_Yaw; }
        void SetYaw(float yaw) { m_Yaw = yaw; RecalculateViewMatrix(); }

    private:
        void RecalculateViewMatrix();
        void RecalculateProjectionMatrix();

        bool OnWindowResize(const Platform::WindowResizedEvent& e);
        bool OnMouseMoved(const Platform::MouseMovedEvent& e);
        bool OnMouseScrolled(const Platform::MouseScrolledEvent& e);

        void MousePan(const glm::vec2& delta);
        void MouseRotate(const glm::vec2& delta);
        void MouseZoom(float delta);

        glm::vec3 CalculatePosition() const;

        std::pair<float, float> PanSpeed() const;
        float RotationSpeed() const { return 0.8f; }
        float ZoomSpeed() const;

    private:
        float m_ViewportWidth = 1280.0f, m_ViewportHeight = 720.0f;

        float m_VerticalFOV = glm::radians(60.0f);
        float m_NearClip = 0.001f, m_FarClip = 1000.0f;
        float m_AspectRatio = 16.0f / 9.0f;

        glm::mat4 m_ViewMatrix = 1.0f;
        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 m_FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec2 m_LastMousePosition = { 0.0f, 0.0f };

        float m_Distance = 10.0f;
        float m_Pitch = 0.0f;
        float m_Yaw = 0.0f;
    };
}
