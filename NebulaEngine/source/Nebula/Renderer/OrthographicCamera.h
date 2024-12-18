#pragma once

#include <glm/glm.hpp>

namespace Nebula {
	/**
	 * Orthographic Camera.
	 */
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);

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
		 * Gets the rotation of the camera.
		 * @return Rotation of the camera in degrees
		 */
		const float getRotation() const { return m_Rotation; }
		/**
		 * Sets the rotation of the camera.
		 * @param rotation Rotation of the camera in degrees
		 */
		void setRotation(const float rotation) { m_Rotation = rotation; RecalculateViewMatrix(); }

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

	private:
		void RecalculateViewMatrix();

	private:
		glm::mat4 m_ProjectionMatrix;
		glm::mat4 m_ViewMatrix;
		glm::mat4 m_ViewProjectionMatrix;

		glm::vec3 m_Position = {0.0f, 0.0f, 0.0f};
		float m_Rotation = 0.0f;
	};

}
