#pragma once

#include <glm/glm.hpp>

namespace Nebula {

	class Camera
	{
	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix)
			: m_ProjectionMatrix(projectionMatrix) {}

		virtual ~Camera() = default;

		[[nodiscard]] const glm::mat4& getProjectionMatrix() const { return m_ProjectionMatrix; }

	protected:
		glm::mat4 m_ProjectionMatrix = 1.0f;

	};
}
