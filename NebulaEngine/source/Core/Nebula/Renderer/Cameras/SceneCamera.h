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
        void SetPerspective(float verticalFOV, float nearClip = 0.001f, float farClip = 1000.0f);

        void SetViewportSize(uint32_t width, uint32_t height);

        [[nodiscard]] ProjectionType GetProjectionType() const { return m_ProjectionType; }
        void SetProjectionType(const ProjectionType type) { m_ProjectionType = type; RecalculateProjection(); }

        [[nodiscard]] float GetOrthographicSize() const { return m_OrthographicSize; }
        void SetOrthographicSize(const float size) { m_OrthographicSize = size; RecalculateProjection(); }

        [[nodiscard]] float GetOrthographicNearClip() const { return m_OrthographicNear; }
        [[nodiscard]] float GetOrthographicFarClip() const { return m_OrthographicFar; }
        void SetOrthographicNearClip(const float nearClip) { m_OrthographicNear = nearClip; RecalculateProjection(); }
        void SetOrthographicFarClip(const float farClip) { m_OrthographicFar = farClip; RecalculateProjection(); }


        [[nodiscard]] float GetPerspectiveVerticalFOV() const { return m_PerspectiveVerticalFOV; }
        void SetPerspectiveVerticalFOV(const float fov) { m_PerspectiveVerticalFOV = fov; RecalculateProjection(); }

        [[nodiscard]] float GetPerspectiveNearClip() const { return m_PerspectiveNear; }
        [[nodiscard]] float GetPerspectiveFarClip() const { return m_PerspectiveFar; }
        void SetPerspectiveNearClip(const float nearClip) { m_PerspectiveNear = nearClip; RecalculateProjection(); }
        void SetPerspectiveFarClip(const float farClip) { m_PerspectiveFar = farClip; RecalculateProjection(); }


    private:
        void RecalculateProjection();

    private:
        ProjectionType m_ProjectionType = ProjectionType::Orthographic;

        float m_OrthographicSize = 10.0f;
        float m_OrthographicNear = -1.0f, m_OrthographicFar = 1.0f;

        float m_PerspectiveVerticalFOV = glm::radians(60.0f);
        float m_PerspectiveNear = 0.001f, m_PerspectiveFar = 1000.0f;

        float m_AspectRatio = 16.0f / 9.0f;
    };

}
