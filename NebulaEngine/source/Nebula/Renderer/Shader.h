#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Nebula {
	/**
	 * Class representing a Shader.
	 */
	class Shader
	{
	public:
		virtual ~Shader() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/**
		 * Sets uniform.
		 * @param name Name of the uniform
		 * @param matrix Mat4 of the uniform
		 */
		virtual void UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const = 0;

		/**
		 * Creates a shader with the specified source code.
		 * @param vertexSrc Vertex shader source code
		 * @param fragmentSrc Fragment shader source code
		 * @return Pointer to the shader
		 */
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc);
	};

}
