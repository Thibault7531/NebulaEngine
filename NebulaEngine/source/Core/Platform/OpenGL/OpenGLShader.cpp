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

	void OpenGLShader::CompileSpirv(const uint32_t* vertexBinary, uint32_t vertexBinarySize, const uint32_t* fragmentBinary, uint32_t fragmentBinarySize)
	{
		NB_PROFILE_FUNCTION();

		GLuint program = glCreateProgram();
		std::array<GLenum, 2> glShaderIDs;
		int glShaderIDIndex = 0;

		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		glShaderBinary(1, &vertexShader, GL_SHADER_BINARY_FORMAT_SPIR_V, vertexBinary, vertexBinarySize);
		glSpecializeShader(vertexShader, "main", 0, nullptr, nullptr);

		GLint success = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(vertexShader);

			NB_CORE_ERROR("{0}", infoLog.data());
			NB_CORE_ASSERT(false, "Shader compilation failed!");
			return;
		}

		glAttachShader(program, vertexShader);
		glShaderIDs[glShaderIDIndex++] = vertexShader;

		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderBinary(1, &fragmentShader, GL_SHADER_BINARY_FORMAT_SPIR_V, fragmentBinary, fragmentBinarySize);
		glSpecializeShader(fragmentShader, "main", 0, nullptr, nullptr);

		success = 0;
		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			glDeleteShader(fragmentShader);

			NB_CORE_ERROR("{0}", infoLog.data());
			NB_CORE_ASSERT(false, "Shader compilation failed!");
			return;
		}

		glAttachShader(program, fragmentShader);
		glShaderIDs[glShaderIDIndex++] = fragmentShader;

		glLinkProgram(program);

		success = 0;
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

		slang::SessionDesc sessionDesc = {};
		slang::TargetDesc targetDesc = {};
		targetDesc.format = SLANG_SPIRV;
		targetDesc.profile = s_GlobalSession->findProfile("spirv_1_5");

		sessionDesc.targets = &targetDesc;
		sessionDesc.targetCount = 1;

		Slang::ComPtr<slang::ISession> session;
		s_GlobalSession->createSession(sessionDesc, session.writeRef());

		session->createCompileRequest(m_CompileRequest.writeRef());

		int targetIndex = m_CompileRequest->addCodeGenTarget(SLANG_SPIRV);
		m_CompileRequest->setTargetProfile(targetIndex, s_GlobalSession->findProfile("spirv_1_5"));

		int translationUnitIdx = m_CompileRequest->addTranslationUnit(SLANG_SOURCE_LANGUAGE_SLANG, m_Name.c_str());
		m_CompileRequest->addTranslationUnitSourceString(translationUnitIdx, m_Name.c_str(), shader.c_str());

		int vertexEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "vertexMain", SLANG_STAGE_VERTEX);
		int fragmentEntryPointIdx = m_CompileRequest->addEntryPoint(translationUnitIdx, "fragmentMain", SLANG_STAGE_FRAGMENT);

		const SlangResult vertexCompileResult = m_CompileRequest->compile();
		if (!SLANG_SUCCEEDED(vertexCompileResult))
		{
			const char* errorMsg = m_CompileRequest->getDiagnosticOutput();
			NB_CORE_ERROR("Slang compilation failed : {0}", errorMsg);
			return;
		}

		m_CompileRequest->getProgramWithEntryPoints(m_ProgramReflection.writeRef());

		Slang::ComPtr<ISlangBlob> vertexBlob;
		m_CompileRequest->getEntryPointCodeBlob(vertexEntryPointIdx, targetIndex, vertexBlob.writeRef());

		Slang::ComPtr<ISlangBlob> fragmentBlob;
		m_CompileRequest->getEntryPointCodeBlob(fragmentEntryPointIdx, targetIndex, fragmentBlob.writeRef());

		CompileSpirv(static_cast<const uint32_t*>(vertexBlob->getBufferPointer()), static_cast<uint32_t>(vertexBlob->getBufferSize()), static_cast<const uint32_t*>(fragmentBlob->getBufferPointer()), static_cast<uint32_t>(fragmentBlob->getBufferSize()));
		GenerateLayout();

		m_IsSlangShader = true;
	}

	void OpenGLShader::GenerateLayout()
	{
		constexpr uint32_t MAX_BINDINGS_PER_SET = 16;
		slang::ProgramLayout* programLayout = m_ProgramReflection->getLayout();
		uint32_t paramCount = programLayout->getParameterCount();

		for (uint32_t i = 0; i < paramCount; i++)
		{
			auto varLayout = programLayout->getParameterByIndex(i);
			auto var = varLayout->getVariable();
			auto typeLayout = varLayout->getTypeLayout();

			ResourceBinding res{};
			res.Name = var->getName();
			res.Size = typeLayout->getSize();
			res.Set = varLayout->getBindingSpace();
			res.Binding = (res.Set * MAX_BINDINGS_PER_SET) + varLayout->getBindingIndex();

			switch (typeLayout->getKind())
			{
			case slang::TypeReflection::Kind::ConstantBuffer:
			case slang::TypeReflection::Kind::ParameterBlock:
			case slang::TypeReflection::Kind::Struct:
				{
					res.Type = ResourceType::UniformBuffer;
					break;
				}

			case slang::TypeReflection::Kind::Resource:
				{
					res.Type = ResourceType::Texture;
					break;
				}

			case slang::TypeReflection::Kind::SamplerState:
				{
					res.Type = ResourceType::Sampler;
					break;
				}

			default: continue;
			}

			m_Layout.ResourceIndex[res.Name] = m_Layout.ResourceIndex.size();
			m_Layout.Resources.push_back(res);
		}
	}

	uint32_t OpenGLShader::GetUniformBlockLocation(const std::string& name)
	{
		ResourceBinding resource = m_Layout.Resources[m_Layout.ResourceIndex[name]];
		return resource.Binding;
	}
}
