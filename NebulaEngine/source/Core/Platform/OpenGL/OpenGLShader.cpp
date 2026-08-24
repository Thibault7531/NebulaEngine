#include "OpenGLShader.h"

#include <regex>
#include <slang.h>
#include <glm/gtc/type_ptr.hpp>

namespace Nebula::Graphics {
	bool OpenGLShader::s_IsSlangGlobalSessionCreated = false;
	Slang::ComPtr<slang::IGlobalSession> OpenGLShader::s_GlobalSession;

	static GLenum ShaderTypeFromString(const std::string& type)
	{
		if (type == "vertex") return GL_VERTEX_SHADER;
		if (type == "fragment" || type == "pixel") return GL_FRAGMENT_SHADER;

		NB_CORE_ASSERT(false, "Unknown shader type !");
		return 0;
	}

	OpenGLShader::OpenGLShader(const std::filesystem::path& filepath)
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

	OpenGLShader::~OpenGLShader()
	{
		NB_PROFILE_FUNCTION();

		glDeleteProgram(m_RendererID);
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

	void OpenGLShader::CompileGLSL(const std::unordered_map<GLenum, std::string>& shaderSources)
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

	void OpenGLShader::CompileSlang(const std::string& shader)
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
		targetDesc.profile = s_GlobalSession->findProfile("glsl_460");

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

	uint32_t OpenGLShader::GetUniformBlockLocation(const std::string& name) const
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
