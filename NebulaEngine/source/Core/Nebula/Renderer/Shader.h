#pragma once

#include <filesystem>
#include <string>
#include <glm/glm.hpp>

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula {

	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void SetInt(const std::string& name, int value) = 0;
		virtual void SetIntArray(const std::string& name, int* value, uint32_t count) = 0;
		virtual void SetFloat(const std::string& name, float value) = 0;
		virtual void SetFloat3(const std::string& name, const glm::vec3& value) = 0;
		virtual void SetFloat4(const std::string& name, const glm::vec4& value) = 0;
		virtual void SetMat4(const std::string& name, const glm::mat4& value) = 0;


		[[nodiscard]] virtual const std::string& GetName() const = 0;

		static Ref<Shader> Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc);
		static Ref<Shader> Create(const std::string& name, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);
		static Ref<Shader> Create(const std::filesystem::path& filepath);

	};

	class ShaderLibrary
	{
	public:
		void Add(const std::string& name, const Ref<Shader>& shader);
		void Add(const Ref<Shader>& shader);
		Ref<Shader> Load(const std::filesystem::path& filepath);
		Ref<Shader> Load(const std::string& name, const std::filesystem::path& filepath);
		Ref<Shader> Load(const std::string& name, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath);

		Ref<Shader> Get(const std::string& name);

		[[nodiscard]] bool Exists(const std::string& name) const;
	private:
		std::unordered_map<std::string, Ref<Shader>> m_Shaders;
	};

}
