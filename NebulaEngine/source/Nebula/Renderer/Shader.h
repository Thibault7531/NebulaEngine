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
		 * Gets the name of the shader.
		 * @return Name of the shader
		 */
		virtual const std::string& GetName() const = 0;

		/**
		 * Creates a shader with the specified source code.
		 * @param vertexSrc Vertex shader source code or file path
		 * @param fragmentSrc Fragment shader source code or file path
		 * @param sourceType Specifies if inputs are file path or sources.
		 * @return Pointer to the shader
		 */
		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, SOURCE_TYPE sourceType);
		/**
		 * Creates a shader with the specified source code.
		 * @param filepath Path to the unified shader file
		 * @return Pointer to the shader.
		 */
		static Ref<Shader> Create(const std::string& filepath);

	};

	class ShaderLibrary
	{
	public:
		/**
		 * Adds a shader to the library.
		 * @param name Name of the shader to add
		 * @param shader Shader to add
		 */
		void Add(const std::string& name, const Ref<Shader>& shader);
		/**
		 * Adds a shader to the library.
		 * @param shader Shader to add
		 */
		void Add(const Ref<Shader>& shader);
		/**
		 * Loads a shader into the library.
		 * @param filepath Path to the shader file to load
		 * @return Loaded shader
		 */
		Ref<Shader> Load(const std::string& filepath);
		/**
		 * Loads a shader into the library.
		 * @param name Name of the shader to load.
		 * @param filepath Path to the shader file to load
		 * @return Loaded shader
		 */
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		/**
		 * Loads a shader into the library.
		 * @param name Name of the shader to load.
		 * @param vertexPath Path to the vertex shader file to load
		 * @param fragmentPath Path to the fragment shader file to load
		 * @return Loaded shader
		 */
		Ref<Shader> Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

		/**
		 * Gets a shader from the library.
		 * @param name Name of the shader
		 * @return Shader
		 */
		Ref<Shader> Get(const std::string& name);

		/**
		 * Asks if the shader is in the library.
		 * @param name name of the shader
		 * @return if the shader is in the library
		 */
		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}
