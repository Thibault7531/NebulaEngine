#include "OpenGL410Shader.h"

#include <regex>
#include <glm/gtc/type_ptr.hpp>

namespace Nebula {
	bool OpenGL410Shader::s_IsSlangGlobalSessionCreated = false;
	Slang::ComPtr<slang::IGlobalSession> OpenGL410Shader::s_GlobalSession;

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		NB_CORE_ASSERT(false, "Unknown shader type !");
		return 0;
	}

	OpenGL410Shader::OpenGL410Shader(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
		: m_Name(name)
	{
		NB_PROFILE_FUNCTION();
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = vertexSrc;
		shaderSources[GL_FRAGMENT_SHADER] = fragmentSrc;
		CompileGLSL(shaderSources);
	}

	OpenGL410Shader::OpenGL410Shader(const std::string& name, const std::filesystem::path& vertexPath,
		const std::filesystem::path& fragmentPath)
			: m_Name(name)
	{
		std::unordered_map<GLenum, std::string> shaderSources;
		shaderSources[GL_VERTEX_SHADER] = ReadFile(vertexPath.string());
		shaderSources[GL_FRAGMENT_SHADER] = ReadFile(fragmentPath.string());
		CompileGLSL(shaderSources);
	}

	OpenGL410Shader::OpenGL410Shader(const std::filesystem::path& filepath)
	{
		NB_PROFILE_FUNCTION();

		m_Name = filepath.stem().string();
		std::string shaderSource = ReadFile(filepath.string());

		if (filepath.extension() == ".slang") CompileSlang(shaderSource);
		else
		{
			auto shaderSources = PreProcess(shaderSource);
			CompileGLSL(shaderSources);
		}
	}

	OpenGL410Shader::~OpenGL410Shader()
	{
		NB_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
	}

	void OpenGL410Shader::Bind() const
	{
		NB_PROFILE_FUNCTION();

		glUseProgram(m_RendererID);
	}

	void OpenGL410Shader::Unbind() const
	{
		NB_PROFILE_FUNCTION();

		glUseProgram(0);
	}

	void OpenGL410Shader::SetInt(const std::string& name, const int value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformInt(name, value);
	}

	void OpenGL410Shader::SetIntArray(const std::string &name, int *value, const uint32_t count)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformIntArray(name, value, count);
	}

	void OpenGL410Shader::SetFloat(const std::string &name, const float value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat(name, value);
	}

	void OpenGL410Shader::SetFloat3(const std::string& name, const glm::vec3& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat3(name, value);
	}

	void OpenGL410Shader::SetFloat4(const std::string& name, const glm::vec4& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformFloat4(name, value);
	}

	void OpenGL410Shader::SetMat4(const std::string& name, const glm::mat4& value)
	{
		NB_PROFILE_FUNCTION();

		UploadUniformMat4(name, value);
	}

	void OpenGL410Shader::SetUniformBlock(const std::string& name, Ref<UniformBuffer> ubo)
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformBlockIndex(m_RendererID, name.c_str());
		ubo->BindToSlot(location);
	}

	void OpenGL410Shader::UploadUniformInt(const std::string& name, int value) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1i(location, value);
	}

	void OpenGL410Shader::UploadUniformIntArray(const std::string &name, const int *value, const uint32_t count) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1iv(location, count, value);
	}

	void OpenGL410Shader::UploadUniformFloat(const std::string& name, float value) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform1f(location, value);
	}

	void OpenGL410Shader::UploadUniformFloat2(const std::string& name, const glm::vec2& vector) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform2fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGL410Shader::UploadUniformFloat3(const std::string& name, const glm::vec3& vector) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform3fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGL410Shader::UploadUniformFloat4(const std::string& name, const glm::vec4& vector) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniform4fv(location, 1, glm::value_ptr(vector));
	}

	void OpenGL410Shader::UploadUniformMat3(const std::string& name, const glm::mat3& matrix) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix3fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	void OpenGL410Shader::UploadUniformMat4(const std::string& name, const glm::mat4& matrix) const
	{
		GLint location = 0;
		if (m_IsSlangShader) location = GetSlangUniformLocation(name);
		else location = glGetUniformLocation(m_RendererID, name.c_str());
		glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
	}

	std::string OpenGL410Shader::ReadFile(const std::string& filepath)
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

	std::unordered_map<GLenum, std::string> OpenGL410Shader::PreProcess(const std::string& source)
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

	void OpenGL410Shader::CompileGLSL(const std::unordered_map<GLenum, std::string>& shaderSources)
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

	void OpenGL410Shader::CompileSlang(const std::string& shader)
	{
		if (!s_IsSlangGlobalSessionCreated)
		{
			slang::createGlobalSession(s_GlobalSession.writeRef());

			s_IsSlangGlobalSessionCreated = true;
		}

		std::unordered_map<GLenum, std::string> shaderSources;

		slang::SessionDesc sessionDesc = {};
		slang::TargetDesc targetDesc = {};
		targetDesc.format = SLANG_GLSL;
		targetDesc.profile = s_GlobalSession->findProfile("glsl_410");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<slang::ISession> session;
		s_GlobalSession->createSession(sessionDesc, session.writeRef());

		session->createCompileRequest(m_CompileRequest.writeRef());

		int translationUnitIdx = m_CompileRequest->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, m_Name.c_str());
		m_CompileRequest->addTranslationUnitSourceString(translationUnitIdx, m_Name.c_str(), shader.c_str());

		int vertexEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "vertexMain", SLANG_STAGE_VERTEX);
		int fragmentEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "fragmentMain", SLANG_STAGE_FRAGMENT);

		const SlangResult vertexCompileResult = m_CompileRequest->compile();
		NB_CORE_ASSERT(SLANG_SUCCEEDED(vertexCompileResult), "Slang compilation failed !");

		m_CompileRequest->getProgramWithEntryPoints(m_ProgramReflection.writeRef());

		std::regex setFirstPattern(R"(\bset\s*=\s*\d+\s*,\s*)");
		std::regex setLastPattern(R"(,\s*\bset\s*=\s*\d+)");
		size_t outputSize = 0;

		std::string vertexSource = static_cast<const char*>(m_CompileRequest->getEntryPointCode(vertexEntryPointIdx, &outputSize));
		std::string cleanedVertexSource = std::regex_replace(vertexSource, setFirstPattern, "");
		cleanedVertexSource = std::regex_replace(cleanedVertexSource, setLastPattern, "");

		std::string fragmentSource = static_cast<const char*>(m_CompileRequest->getEntryPointCode(fragmentEntryPointIdx, &outputSize));
		std::string cleanedFragmentSource = std::regex_replace(fragmentSource, setFirstPattern, "");
		cleanedFragmentSource = std::regex_replace(cleanedFragmentSource, setLastPattern, "");

		shaderSources[GL_VERTEX_SHADER] = cleanedVertexSource;
		shaderSources[GL_FRAGMENT_SHADER] = cleanedFragmentSource;

		CompileGLSL(shaderSources);

		m_IsSlangShader = true;
	}

	uint32_t OpenGL410Shader::GetSlangUniformLocation(const std::string& name) const
	{
		slang::ProgramLayout* programLayout = m_ProgramReflection->getLayout(0);
		uint32_t paramCount = programLayout->getParameterCount();

		uint32_t paramIndex = 0;
		slang::VariableLayoutReflection* uniformLayout = nullptr;
		while (paramIndex < paramCount && !uniformLayout)
		{
			slang::VariableLayoutReflection* paramLayout = programLayout->getParameterByIndex(paramIndex);
			if (strcmp(paramLayout->getVariable()->getName(), name.c_str()) == 0) uniformLayout = paramLayout;
			paramIndex++;
		}

		if (uniformLayout)
		{
			slang::TypeReflection* type = uniformLayout->getTypeLayout()->getType();
			switch (type->getKind())
			{
			case slang::TypeReflection::Kind::Resource:
				{
					SlangResourceShape shape = type->getResourceShape();
					uint32_t baseShape = shape & SLANG_RESOURCE_BASE_SHAPE_MASK;

					if (baseShape == SLANG_TEXTURE_2D)
					{
						return uniformLayout->getOffset(slang::ParameterCategory::DescriptorTableSlot);
					}
					return 0;
				}

			case slang::TypeReflection::Kind::ConstantBuffer:
			case slang::TypeReflection::Kind::ParameterBlock:
			case slang::TypeReflection::Kind::Struct:
				{
					return uniformLayout->getOffset(slang::ParameterCategory::DescriptorTableSlot);
				}

			case slang::TypeReflection::Kind::Scalar:
			case slang::TypeReflection::Kind::Vector:
			case slang::TypeReflection::Kind::Matrix:
				{
					uint32_t categoryCount = uniformLayout->getCategoryCount();
					for (uint32_t i = 0; i < categoryCount; i++)
					{
						slang::ParameterCategory category = uniformLayout->getCategoryByIndex(i);
						if (category == slang::ParameterCategory::Uniform) return uniformLayout->getOffset(category);
					}
				}

			default:
				{
					return 0;
				}
			}
		}
		return 0;
	}
}
