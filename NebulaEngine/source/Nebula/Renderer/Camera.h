#pragma once

#include <glm/glm.hpp>

namespace Nebula {
	/**
	 * Base camera class
	 */
	class Camera
	{
	public:
		/**
		 * Gets the position of the camera.
		 * @return Vec3 of the position of the camera
		 */
		const glm::vec3& getPosition() const { return m_Position; }
		/**
		 * Sets the position of the camera.
		 * @param position Vec3 of the position of the camera
		 */
		void setPosition(const glm::vec3& position) { m_Position = position; RecalculateViewMatrix(); }

		/**
		 * Gets the projection matrix.
		 * @return Mat4 of the projection matrix
		 */
		const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }
		/**
		 * Gets the view matrix.
		 * @return Mat4 of the view matrix
		 */
		const glm::mat4& getViewMatrix() const { return m_ViewMatrix; }
		/**
		 * Gets the view projection matrix.
		 * @return Mat4 of the view projection matrix
		 */
		const glm::mat4& getViewProjectionMatrix() const { return m_ViewProjectionMatrix; }

	protected:
		virtual void RecalculateViewMatrix() = 0;

	protected:
		glm::mat4 m_ProjectionMatrix = 1.0f;
		glm::mat4 m_ViewMatrix = 1.0f;
		glm::mat4 m_ViewProjectionMatrix = 1.0f;

		glm::vec3 m_Position = { 0.0f, 0.0f, 0.0f };
	};

	/**
	 * Orthographic Camera.
	 */
	class OrthographicCamera : public Camera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

		/**
		 * Gets the rotation of the camera.
		 * @return Rotation of the camera in degrees
		 */
		const float getRotation() const { return m_Rotation; }
		/**
		 * Sets the rotation of the camera.
		 * @param rotation Rotation of the camera in degrees
		 */
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

		/**
		 * Gets the aspect ratio of the camera.
		 * @return Aspect ratio of the camera
		 */
		const float getAspectRatio() const { return m_AspectRatio; }
		/**
		 * Sets the aspect ratio of the camera.
		 * @param aspectRatio Aspect ratio of the camera
		 */
		void setAspectRatio(float aspectRatio) { m_AspectRatio = aspectRatio; RecalculateProjectionMatrix(); }

		/**
		 * Gets the FOV of the camera.
		 * @return FOV of the camera
		 */
		const float getFOV() const { return m_FOV; }
		/**
		 * Sets the FOV of the camera.
		 * @param FOV FOV of the camera
		 */
		void setFOV(const float FOV) { m_FOV = FOV; RecalculateProjectionMatrix(); }

		/**
		 * Gets the rotation of the camera.
		 * @return Rotation of the camera
		 */
		const glm::vec3& getRotation() const { return m_Rotation; }
		/**
		 * Sets the rotation of the camera.
		 * @param rotation Rotation of the camera
		 */
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
