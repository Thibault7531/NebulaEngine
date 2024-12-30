#include "CameraController.h"

#include "Input.h"
#include "KeyCodes.h"

namespace Nebula
{
    OrthographicCameraController::OrthographicCameraController(float aspectRatio, bool rotation)
        : m_aspectRatio(aspectRatio), m_Camera(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel), m_Rotation(rotation)
    {
    }

    void OrthographicCameraController::OnUpdate(Timestep ts)
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

    bool OrthographicCameraController::OnMouseScrolled(MouseScrolledEvent& e)
    {
        m_zoomLevel -= e.GetYOffset() * 0.25f;
        m_zoomLevel = std::max(m_zoomLevel, 0.25f);
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }

    bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
    {
        m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }
}
