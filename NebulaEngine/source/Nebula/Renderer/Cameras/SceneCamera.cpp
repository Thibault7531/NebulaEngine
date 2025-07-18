#include "SceneCamera.h"

#include "ext/matrix_clip_space.hpp"

namespace Nebula
{

    SceneCamera::SceneCamera()
    {
        RecalculateProjection();
    }

    void SceneCamera::SetOrthographic(float size, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Orthographic;

        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;

        RecalculateProjection();
    }

    void SceneCamera::SetPerspective(float verticalFOV, float nearClip, float farClip)
    {
        m_ProjectionType = ProjectionType::Perspective;

        m_PerspectiveVerticalFOV = verticalFOV;
        m_PerspectiveNear = nearClip;
        m_PerspectiveFar = farClip;

        RecalculateProjection();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = static_cast<float>(width) / static_cast<float>(height);

        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        switch (m_ProjectionType)
        {
            case ProjectionType::Orthographic:
                {
                    m_ProjectionMatrix = glm::ortho(-.5f * m_AspectRatio * m_OrthographicSize, .5f * m_AspectRatio * m_OrthographicSize, -.5f * m_OrthographicSize, .5f * m_OrthographicSize, m_OrthographicNear, m_OrthographicFar);
                    break;
                }
            case ProjectionType::Perspective:
                {
                    m_ProjectionMatrix = glm::perspective(m_PerspectiveVerticalFOV, m_AspectRatio, m_PerspectiveNear, m_PerspectiveFar);
                    break;
                }
        }
    }

}
