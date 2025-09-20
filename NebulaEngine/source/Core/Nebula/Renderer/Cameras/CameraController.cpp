#include "CameraController.h"

#include "Nebula/CoreSystems/Input/Input.h"
#include "Nebula/CoreSystems/Input/KeyCodes.h"
#include "Nebula/CoreSystems/Input/MouseButtonCodes.h"
#include "OrthographicCamera.h"

namespace Nebula
{
    OrthographicCameraController::OrthographicCameraController(const float aspectRatio, const bool rotation)
        : m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnResize(float width, float height)
    {
        m_aspectRatio = width / height;
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);
    }

    void OrthographicCameraController::OnUpdate(const Timestep ts)
    {
        NB_PROFILE_FUNCTION();

        if (Input::IsKeyPressed(KeyboardKey::Z)) {
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(KeyboardKey::S)) {
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(KeyboardKey::D)) {
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(KeyboardKey::Q)) {
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
        }

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(KeyboardKey::A)) {
                m_CameraRotation += m_CameraRotationSpeed * ts;
            }
            if (Input::IsKeyPressed(KeyboardKey::E)) {
                m_CameraRotation -= m_CameraRotationSpeed * ts;
            }

            m_Camera.SetRotation(m_CameraRotation);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        NB_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
    }

    bool OrthographicCameraController::OnMouseScrolled(const MouseScrolledEvent& e)
    {
        NB_PROFILE_FUNCTION();

        m_zoomLevel -= e.GetYOffset() * 0.25f;
        m_zoomLevel = std::max(m_zoomLevel, 0.25f);
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }

    bool OrthographicCameraController::OnWindowResize(const WindowResizeEvent& e)
    {
        NB_PROFILE_FUNCTION();

        OnResize(e.GetWidth(), e.GetHeight());

        return false;
    }

    PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float FOV)
        : m_aspectRatio(aspectRatio), m_FOV(FOV), m_Camera(PerspectiveCamera(m_FOV, m_aspectRatio))
    {
    }

    void PerspectiveCameraController::OnUpdate(const Timestep ts)
    {
        NB_PROFILE_FUNCTION();

        if (Input::IsKeyPressed(KeyboardKey::Z)) {
            m_CameraPosition += glm::vec3(2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(KeyboardKey::S)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(KeyboardKey::D)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(KeyboardKey::Q)) {
            m_CameraPosition += glm::vec3(2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(KeyboardKey::Space)) {
            m_CameraPosition += glm::vec3(0.0f, 2.0f * ts, 0.0f);
        }
        if (Input::IsKeyPressed(KeyboardKey::LeftShift)) {
            m_CameraPosition += glm::vec3(0.0f, -2.0f * ts, 0.0f);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void PerspectiveCameraController::OnEvent(Event& e)
    {
        NB_PROFILE_FUNCTION();

        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoveEvent));
        dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));

        m_PreviousMousePosition = {Input::GetMouseX(), Input::GetMouseY()};
    }

    bool PerspectiveCameraController::OnMouseMoveEvent(const MouseMovedEvent& e)
    {
        NB_PROFILE_FUNCTION();

        if (Input::IsMouseButtonPressed(MouseButton::Left))
        {
            m_cameraRotation += glm::vec3(m_PreviousMousePosition.y - e.GetY(), e.GetX() - m_PreviousMousePosition.x, 0.0f) * 0.1f;
            m_Camera.SetRotation(m_cameraRotation);
        }

        return false;
    }

    bool PerspectiveCameraController::OnWindowResize(const WindowResizeEvent& e)
    {
        NB_PROFILE_FUNCTION();

        m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
        m_Camera.SetAspectRatio(m_aspectRatio);

        return false;
    }
}
