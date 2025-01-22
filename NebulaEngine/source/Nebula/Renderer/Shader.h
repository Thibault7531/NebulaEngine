#pragma once

#include <string>
#include <glm/glm.hpp>

namespace Nebula {

	class Shader
	{
	public:
		enum class SOURCE_TYPE { NB_SHADER_STRING, NB_SHADER_FILE};
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;


		virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, SOURCE_TYPE sourceType);
		static Ref<Shader> Create(const std::string& filepath);

	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& filepath);
		Ref<Shader> Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath);

		Ref<Shader> Get(const std::string& name);

		bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}
