#include "CameraController.h"

#include "../Core/Input.h"
#include "../Core/KeyCodes.h"
#include "../Core/MouseButtonCodes.h"

namespace Nebula
{
    OrthographicCameraController::OrthographicCameraController(const float aspectRatio, const bool rotation)
        : m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(const Timestep ts)
    {
        if (Input::IsKeyPressed(NB_KEY_Z)) {
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(NB_KEY_S)) {
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(NB_KEY_D)) {
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;
        }
        if (Input::IsKeyPressed(NB_KEY_Q)) {
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
        }

        if (m_Rotation)
        {
            if (Input::IsKeyPressed(NB_KEY_A)) {
                m_CameraRotation += m_CameraRotationSpeed * ts;
            }
            if (Input::IsKeyPressed(NB_KEY_E)) {
                m_CameraRotation -= m_CameraRotationSpeed * ts;
            }

            m_Camera.setRotation(m_CameraRotation);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void OrthographicCameraController::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseScrolledEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
    }

    bool OrthographicCameraController::OnMouseScrolled(const MouseScrolledEvent& e)
    {
        m_zoomLevel -= e.GetYOffset() * 0.25f;
        m_zoomLevel = std::max(m_zoomLevel, 0.25f);
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }

    bool OrthographicCameraController::OnWindowResize(const WindowResizeEvent& e)
    {
        m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }

    PerspectiveCameraController::PerspectiveCameraController(float aspectRatio, float FOV)
        : m_aspectRatio(aspectRatio), m_FOV(FOV), m_Camera(PerspectiveCamera(m_FOV, m_aspectRatio))
    {
    }

    void PerspectiveCameraController::OnUpdate(const Timestep ts)
    {
        if (Input::IsKeyPressed(NB_KEY_Z)) {
            m_CameraPosition += glm::vec3(2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(NB_KEY_S)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(NB_KEY_D)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(NB_KEY_Q)) {
            m_CameraPosition += glm::vec3(2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Input::IsKeyPressed(NB_KEY_SPACE)) {
            m_CameraPosition += glm::vec3(0.0f, 2.0f * ts, 0.0f);
        }
        if (Input::IsKeyPressed(NB_KEY_LSHIFT)) {
            m_CameraPosition += glm::vec3(0.0f, -2.0f * ts, 0.0f);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void PerspectiveCameraController::OnEvent(Event& e)
    {
        EventDispatcher dispatcher(e);
        dispatcher.Dispatch<MouseMovedEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoveEvent));
        dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));

        m_PreviousMousePosition = {Input::GetMouseX(), Input::GetMouseY()};
    }

    bool PerspectiveCameraController::OnMouseMoveEvent(const MouseMovedEvent& e)
    {
        if (Input::IsMouseButtonPressed(NB_BUTTON_LEFT))
        {
            m_cameraRotation += glm::vec3(m_PreviousMousePosition.y - e.GetY(), e.GetX() - m_PreviousMousePosition.x, 0.0f) * 0.1f;
            m_Camera.setRotation(m_cameraRotation);
        }

        return false;
    }

    bool PerspectiveCameraController::OnWindowResize(const WindowResizeEvent& e)
    {
        m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
        m_Camera.SetProjection(m_aspectRatio);

        return false;
    }
}
