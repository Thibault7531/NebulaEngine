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
		enum class SOURCE_TYPE { NB_SHADER_STRING, NB_SHADER_FILE};
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/**
		 * Creates a shader with the specified source code.
		 * @param vertexSrc Vertex shader source code or file path
		 * @param fragmentSrc Fragment shader source code or file path
		 * @param sourceType Specifies if inputs are file path or sources.
		 * @return Pointer to the shader
		 */
		static Shader* Create(const std::string& vertexSrc, const std::string& fragmentSrc, SOURCE_TYPE sourceType);
		/**
		 * Creates a shader with the specified source code.
		 * @param filepath Path to the unified shader file
		 * @return Pointer to the shader.
		 */
		static Shader* Create(const std::string& filepath);

	};

}
