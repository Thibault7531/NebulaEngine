#include "Camera.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Nebula {

	OrthographicCamera::OrthographicCamera(const float left, const float right, const float bottom, const float top)
	{
		m_ProjectionMatrix = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewMatrix = 1.0f;
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void OrthographicCamera::RecalculateViewMatrix()
	{
		const glm::mat4 transform = glm::translate(glm::mat4(1.0f), m_Position) * glm::rotate(glm::mat4(1.0f), glm::radians(m_Rotation), glm::vec3(0, 0, 1));

		m_ViewMatrix = glm::inverse(transform);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	PerspectiveCamera::PerspectiveCamera(const float FOV, const float aspectRatio)
		: m_AspectRatio(aspectRatio), m_FOV(FOV)
	{
		m_ProjectionMatrix = glm::perspective(glm::radians(FOV), aspectRatio, 0.1f, 100.0f);
		PerspectiveCamera::RecalculateViewMatrix();
	}

	void PerspectiveCamera::RecalculateProjectionMatrix()
	{
		m_ProjectionMatrix = glm::perspective(m_FOV, m_AspectRatio, 0.1f, 100.0f);
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

	void PerspectiveCamera::setRotation(const glm::vec3& rotation)
	{
		m_Rotation = rotation;

		if (m_Rotation.x > 89.9f) m_Rotation.x = 89.9f;
		if (m_Rotation.x < -89.9f) m_Rotation.x = -89.9f;

		RecalculateViewMatrix();
	}

	void PerspectiveCamera::RecalculateViewMatrix()
	{
		glm::vec3 direction;
		direction.x = cos(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
		direction.y = sin(glm::radians(m_Rotation.x));
		direction.z = sin(glm::radians(m_Rotation.y)) * cos(glm::radians(m_Rotation.x));
		direction = glm::normalize(direction);

		m_ViewMatrix = glm::lookAt(m_Position, m_Position + direction, glm::vec3(0, 1, 0));
		m_ViewProjectionMatrix = m_ProjectionMatrix * m_ViewMatrix;
	}

}
