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
        m_OrthographicSize = size;
        m_OrthographicNear = nearClip;
        m_OrthographicFar = farClip;

        RecalculateProjection();
    }

    void SceneCamera::SetViewportSize(uint32_t width, uint32_t height)
    {
        m_AspectRatio = (float)width / (float)height;

        RecalculateProjection();
    }

    void SceneCamera::RecalculateProjection()
    {
        m_ProjectionMatrix = glm::ortho(-.5f * m_AspectRatio * m_OrthographicSize, .5f * m_AspectRatio * m_OrthographicSize, -.5f * m_OrthographicSize, .5f * m_OrthographicSize, m_OrthographicNear, m_OrthographicFar);
    }

}
