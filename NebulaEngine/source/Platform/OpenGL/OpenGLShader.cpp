#include "OpenGLShader.h"

#include <glm/gtc/type_ptr.hpp>

#include "spdlog/fmt/bundled/ranges.h"

namespace Nebula {

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		NB_CORE_ASSERT(false, "Unknown shader type !");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, const SOURCE_TYPE sourceType)
		: m_Name(name)
	{
		NB_PROFILE_FUNCTION();

		std::string VertexSource;
		std::string FragmentSource;
		if (sourceType == SOURCE_TYPE::NB_SHADER_FILE)
		{
			VertexSource = ReadFile(vertexSrc);
			FragmentSource = ReadFile(fragmentSrc);
		}
		else
		{
			VertexSource = vertexSrc;
			FragmentSource = fragmentSrc;
		}

		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = VertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = FragmentSource;
		Compile(shaderSources);
	}

	OpenGLShader::OpenGLShader(const std::string& filepath)
	{
		NB_PROFILE_FUNCTION();

		std::string shaderSource = ReadFile(filepath);
        auto shaderSources = PreProcess(shaderSource);
		Compile(shaderSources);

		auto lastSlash = filepath.find_last_of("\\/");
		lastSlash = lastSlash == std::string::npos ? 0 : lastSlash + 1;
		auto lastDot = filepath.rfind('.');
		auto count = lastDot == std::string::npos ? filepath.size() - lastSlash : lastDot - lastSlash;
		m_Name = filepath.substr(lastSlash, count);
	}

	OpenGLShader::~OpenGLShader()
	{
		NB_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGLShader::Bind() const
	{
		NB_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGLShader::Unbind() const
	{
		NB_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGLShader::SetInt(const std::string& name, const int value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGLShader::SetFloat(const std::string &name, const float value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGLShader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGLShader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGLShader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& name, int value)
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& name, float value) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGLShader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
	{
		const GLint location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	std::string OpenGLShader::ReadFile(const std::string& filepath)
	{
		NB_PROFILE_FUNCTION();

		std::string result;
		std::ifstream in(filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&result[0], result.size());
			in.close();
		}
		else
		{
			NB_CORE_ERROR("Could not open file '{0}'", filepath);
		}

		return result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& source)
	{
		NB_PROFILE_FUNCTION();

		std::unordered_map<GLenum, std::string> result;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = source.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = source.find_first_of("\r\n", pos);
			NB_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = source.substr(begin, eol - begin);
			NB_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specifier: {0}", source);

			size_t nextLinePos = source.find_first_of("\r\n", eol);
			pos = source.find(typeToken, nextLinePos);
			result[ShaderTypeFromString(type)] = source.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? source.size() - 1 : nextLinePos));
		}

		return result;
	}

	void OpenGLShader::Compile(const std::unordered_map<GLenum, std::string>& shaderSources)
	{
		NB_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		NB_CORE_ASSERT(shaderSources.size() <= 2, "Only 2 shaders are supported in a program")
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		for (auto& kv : shaderSources)
		{
			GLenum shaderType = kv.first;
			const std::string& source = kv.second;

			GLuint shader = glCreateShader(shaderType);

			const GLchar* sourceCStr = source.c_str();
			glShaderSource(shader, 1, &sourceCStr, nullptr);

			glCompileShader(shader);

			GLint success = 0;
			glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
			if (success == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

				glDeleteShader(shader);

				NB_CORE_ERROR("{0}", infoLog.data());
				NB_CORE_ASSERT(false, "Shader compilation failed!");
				return;
			}

			glAttachShader(program, shader);
			glShaderIDs[glShaderIDIndex++] = shader;
		}

		glLinkProgram(program);

		GLint success = 0;
		glGetProgramiv(program, GL_LINK_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);

			glDeleteProgram(program);
			for (auto id : glShaderIDs)
			{
				glDeleteShader(id);
			}

			NB_CORE_ERROR("{0}", infoLog.data());
			NB_CORE_ASSERT(false, "Shader program linking failed!");
			return;
		}

		for (auto id : glShaderIDs)
		{
			glDetachShader(program, id);
		}

		m_RendererID = program;
	}
}
