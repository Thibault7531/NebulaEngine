#pragma once

#include <glm.hpp>

namespace Nebula
{
    class OrthographicCamera
    {
    public:
        OrthographicCamera(float left, float right, float bottom, float top);

        [[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
        [[nodiscard]] const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
        [[nodiscard]] const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

        [[nodiscard]] const glm::vec3& getPosition() const { return m_Position; }
        void setPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

        void SetProjection(float left, float right, float bottom, float top);

        [[nodiscard]] float GetRotation() const { return m_Rotation; }
        void SetRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

    private:
        void RecalculateViewMatrix();

    private:
        glm::mat4 m_ProjectionMatrix = 1.0f;
        glm::mat4 m_ViewMatrix = 1.0f;
        glm::mat4 m_ViewProjectionMatrix = 1.0f;

        glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
        float m_Rotation = 0.0f;
    };
}
