#include "EditorCamera.h"

#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"

#include <glm/gtc/matrix_transform.hpp>
#include <detail/type_quat.hpp>
#define GLM_ENABLE_EXPERIMENTAL
#include <gtx/quaternion.hpp>

#include "Platform/Nebula/CoreSystems/Input/Input.h"

namespace Nebula
{
    EditorCamera::EditorCamera(float FOV, float aspectRatio, float nearClip, float farClip)
        : m_VerticalFOV{glm::radians(FOV)}, m_AspectRatio{aspectRatio}, m_NearClip{nearClip}, m_FarClip{farClip}
    {
        RecalculateViewMatrix();
    }

    void EditorCamera::OnEvent(Platform::Event& e)
    {
        Platform::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Platform::WindowResizedEvent>(NB_BIND_EVENT_FN(OnWindowResize));
        dispatcher.Dispatch<Platform::MouseMovedEvent>(NB_BIND_EVENT_FN(OnMouseMoved));
        dispatcher.Dispatch<Platform::MouseScrolledEvent>(NB_BIND_EVENT_FN(OnMouseScrolled));
    }

    void EditorCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);

        RecalculateProjectionMatrix();
    }

    glm::vec3 EditorCamera::GetUpVector() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 1.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetRightVector() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(1.0f, 0.0f, 0.0f));
    }

    glm::vec3 EditorCamera::GetForwardVector() const
    {
        return glm::rotate(GetOrientation(), glm::vec3(0.0f, 0.0f, -1.0f));
    }

    glm::quat EditorCamera::GetOrientation() const
    {
        return glm::quat(glm::vec3(-m_Pitch, -m_Yaw, 0.0f));
    }

    bool EditorCamera::OnWindowResize(const Platform::WindowResizedEvent& e)
    {
        SetViewportSize(e.GetWidth(), e.GetHeight());
        return false;
    }

    bool EditorCamera::OnMouseMoved(const Platform::MouseMovedEvent& e)
    {
        if (Platform::Input::IsKeyPressed(KeyboardKey::LeftAlt))
        {
            glm::vec2 delta = {e.GetX() - m_LastMousePosition.x, e.GetY() - m_LastMousePosition.y};

            if (Platform::Input::IsMouseButtonPressed(MouseButton::Left)) MouseRotate(delta * 0.005f);
            if (Platform::Input::IsMouseButtonPressed(MouseButton::Middle)) MousePan(delta * 0.005f);
            if (Platform::Input::IsMouseButtonPressed(MouseButton::Right)) MouseZoom(delta.y * 0.05f);

            RecalculateViewMatrix();
        }
        m_LastMousePosition = { e.GetX(), e.GetY() };

        return false;
    }

    bool EditorCamera::OnMouseScrolled(const Platform::MouseScrolledEvent& e)
    {
        float delta = e.GetYOffset();
        MouseZoom(delta * 0.5f);
        RecalculateViewMatrix();
        return false;
    }

    void EditorCamera::MousePan(const glm::vec2& delta)
    {
        auto [xSpeed, ySpeed] = PanSpeed();
        m_FocalPoint += -GetRightVector() * delta.x * xSpeed * m_Distance;
        m_FocalPoint += GetUpVector() * delta.y * ySpeed * m_Distance;
    }

    void EditorCamera::MouseRotate(const glm::vec2& delta)
    {
        float yawSign = GetUpVector().y > 0 ? 1.0f : -1.0f;
        m_Yaw += yawSign * delta.x * RotationSpeed();
        m_Pitch += delta.y * RotationSpeed();
    }

    void EditorCamera::MouseZoom(float delta)
    {
        m_Distance -= delta * ZoomSpeed();
        if (m_Distance < 1.0f) m_Distance = 1.0f;
    }

    glm::vec3 EditorCamera::CalculatePosition() const
    {
        return m_FocalPoint - GetForwardVector() * m_Distance;
    }

    std::pair<float, float> EditorCamera::PanSpeed() const
    {
        float x = std::min(m_ViewportWidth / 1000, 2.4f);
        float xFactor = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(m_ViewportHeight / 1000.0f, 2.4f);
        float yFactor = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        return { xFactor, yFactor };
    }

    float EditorCamera::ZoomSpeed() const
    {
        float distance = m_Distance * 0.2f;
        distance = std::max(distance, 0.f);

        float speed = distance * distance;
        speed = std::min(speed, 100.f);

        return speed * 0.5f;
    }

    void EditorCamera::RecalculateViewMatrix()
    {
        m_Position = CalculatePosition();
        glm::quat orientation = GetOrientation();

        m_ViewMatrix = glm::translate(glm::mat4(1.0f), m_Position) * glm::toMat4(orientation);
        m_ViewMatrix = glm::inverse(m_ViewMatrix);
    }

    void EditorCamera::RecalculateProjectionMatrix()
    {
        m_ProjectionMatrix = glm::perspective(m_VerticalFOV, m_AspectRatio, m_NearClip, m_FarClip);
    }
}
