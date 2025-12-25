#include "CameraController.h"

#include "OrthographicCamera.h"
#include "Platform/Nebula/CoreSystems/Input/Input.h"

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

        if (Platform::Input::IsKeyPressed(KeyboardKey::Z)) {
            m_CameraPosition.y += m_CameraTranslationSpeed * ts;
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::S)) {
            m_CameraPosition.y -= m_CameraTranslationSpeed * ts;
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::D)) {
            m_CameraPosition.x += m_CameraTranslationSpeed * ts;
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::Q)) {
            m_CameraPosition.x -= m_CameraTranslationSpeed * ts;
        }

        if (m_Rotation)
        {
            if (Platform::Input::IsKeyPressed(KeyboardKey::A)) {
                m_CameraRotation += m_CameraRotationSpeed * ts;
            }
            if (Platform::Input::IsKeyPressed(KeyboardKey::E)) {
                m_CameraRotation -= m_CameraRotationSpeed * ts;
            }

            m_Camera.SetRotation(m_CameraRotation);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void OrthographicCameraController::OnEvent(Platform::Event& e)
    {
        NB_PROFILE_FUNCTION();

        Platform::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Platform::MouseScrolledEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnMouseScrolled));
        dispatcher.Dispatch<Platform::WindowResizedEvent>(NB_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
    }

    bool OrthographicCameraController::OnMouseScrolled(const Platform::MouseScrolledEvent& e)
    {
        NB_PROFILE_FUNCTION();

        m_zoomLevel -= e.GetYOffset() * 0.25f;
        m_zoomLevel = std::max(m_zoomLevel, 0.25f);
        m_Camera.SetProjection(-m_aspectRatio * m_zoomLevel, m_aspectRatio * m_zoomLevel, -m_zoomLevel, m_zoomLevel);

        return false;
    }

    bool OrthographicCameraController::OnWindowResize(const Platform::WindowResizedEvent& e)
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

        if (Platform::Input::IsKeyPressed(KeyboardKey::Z)) {
            m_CameraPosition += glm::vec3(2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::S)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::D)) {
            m_CameraPosition += glm::vec3(-2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, 2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::Q)) {
            m_CameraPosition += glm::vec3(2.0f * glm::sin(glm::radians(m_cameraRotation.y)) * ts, 0.0f, -2.0f * glm::cos(glm::radians(m_cameraRotation.y)) * ts);
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::Space)) {
            m_CameraPosition += glm::vec3(0.0f, 2.0f * ts, 0.0f);
        }
        if (Platform::Input::IsKeyPressed(KeyboardKey::LeftShift)) {
            m_CameraPosition += glm::vec3(0.0f, -2.0f * ts, 0.0f);
        }

        m_Camera.setPosition(m_CameraPosition);
    }

    void PerspectiveCameraController::OnEvent(Platform::Event& e)
    {
        NB_PROFILE_FUNCTION();

        Platform::EventDispatcher dispatcher(e);
        dispatcher.Dispatch<Platform::MouseMovedEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnMouseMoveEvent));
        dispatcher.Dispatch<Platform::WindowResizedEvent>(NB_BIND_EVENT_FN(PerspectiveCameraController::OnWindowResize));

        m_PreviousMousePosition = {Platform::Input::GetMouseX(), Platform::Input::GetMouseY()};
    }

    bool PerspectiveCameraController::OnMouseMoveEvent(const Platform::MouseMovedEvent& e)
    {
        NB_PROFILE_FUNCTION();

        if (Platform::Input::IsMouseButtonPressed(MouseButton::Left))
        {
            m_cameraRotation += glm::vec3(m_PreviousMousePosition.y - e.GetY(), e.GetX() - m_PreviousMousePosition.x, 0.0f) * 0.1f;
            m_Camera.SetRotation(m_cameraRotation);
        }

        return false;
    }

    bool PerspectiveCameraController::OnWindowResize(const Platform::WindowResizedEvent& e)
    {
        NB_PROFILE_FUNCTION();

        m_aspectRatio = static_cast<float>(e.GetWidth()) / static_cast<float>(e.GetHeight());
        m_Camera.SetAspectRatio(m_aspectRatio);

        return false;
    }
}
