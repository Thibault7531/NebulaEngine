#pragma once

#include <glm/glm.hpp>

namespace Nebula {

	class Camera
	{
	public:
		const glm::vec3& getPosition() const { return m_Position; }
		void setPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
		const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	protected:
		virtual void RecalculateViewMatrix() = 0;

	protected:
		glm::mat4 m_ProjectionMatrix = 1.0f;
		glm::mat4 m_ViewMatrix = 1.0f;
		glm::mat4 m_ViewProjectionMatrix = 1.0f;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	};

	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		void SetProjection(float left, float right, float bottom, float top);

		const float getRotation() const { return m_Rotation; }
		void setRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

	private:
		void RecalculateViewMatrix() override;

	private:
		float m_Rotation = 0.0f;
	};

	class PerspectiveCamera : public Camera
	{
	public:
		PerspectiveCamera(const float FOV, const float aspectRatio);

		void SetProjection(const float aspectRatio);

		const float getAspectRatio() const { return m_AspectRatio; }
		void setAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }

		float getFOV() const { return m_FOV; }
		void setFOV(const float FOV) { m_FOV = FOV; RecalculateProjectionMatrix(); }

		const glm::vec3& getRotation() const { return m_Rotation; }
		void setRotation(const glm::vec3& rotation);

	private:
		void RecalculateViewMatrix() override;
		void RecalculateProjectionMatrix();

	private:
		float m_AspectRatio = 16.0f/9.0f;
		float m_FOV = glm::radians(60.0f);
		glm::vec3 m_Rotation = { 0.0f, -90.0f, 0.0f };
	};
}
