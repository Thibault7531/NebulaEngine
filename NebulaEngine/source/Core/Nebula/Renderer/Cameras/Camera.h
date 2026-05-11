#pragma once

#include <glm/glm.hpp>

namespace Nebula {

	class Camera
	{
	public:
		enum class ProjectionType { Perspective = 0, Orthographic = 1 };

	public:
		Camera() = default;
		Camera(const glm::mat4& projectionMatrix)
			: m_ProjectionMatrix(projectionMatrix) {}

		virtual ~Camera() = default;

		[[nodiscard]] const glm::mat4& GetProjectionMatrix() const { return m_ProjectionMatrix; }

	protected:
		glm::mat4 m_ProjectionMatrix = 1.0f;

	};
}
