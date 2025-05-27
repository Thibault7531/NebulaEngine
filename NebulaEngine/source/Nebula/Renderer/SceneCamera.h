#pragma once
#include "Camera.h"

namespace Nebula
{

    class SceneCamera : public Camera
    {
    public:
        SceneCamera();
        virtual ~SceneCamera() = default;

        void SetOrthographic(float size, float nearClip = -1.0f, float farClip = 1.0f);

        void SetViewportSize(uint32_t width, uint32_t height);

        float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(const float size) { m_OrthographicSize = size; RecalculateProjection(); }

    private:
        void RecalculateProjection();

    private:
        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_AspectRatio = 16.0f / 9.0f;
    };

}
