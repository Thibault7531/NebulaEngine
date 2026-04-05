#include "Shader.h"

#include "Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Shader.h"
#else
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL410/OpenGL410Shader.h"
#endif

namespace Nebula {

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc, SOURCE_TYPE sourceType)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexSrc, fragmentSrc, sourceType);
#else
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc, sourceType);
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexSrc, fragmentSrc, sourceType);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410Shader>(filepath);
#else
		case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case RendererAPI::API::OpenGL: return std::make_shared<OpenGLShader>(filepath);
		case RendererAPI::API::OpenGL410: return std::make_shared<OpenGL410Shader>(filepath);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	void ShaderLibrary::Add(const std::string& name, const Ref<Shader>& shader)
	{
		NB_CORE_ASSERT(!Exists(name), "Shader already exists!");
		m_Shaders[name] = shader;
	}

	void ShaderLibrary::Add(const Ref<Shader>& shader)
	{
		auto& name = shader->GetName();
		Add(name, shader);
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::string& vertexPath, const std::string& fragmentPath)
	{
		auto shader = Shader::Create(name, vertexPath, fragmentPath, Shader::SOURCE_TYPE::NB_SHADER_FILE);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Get(const std::string& name)
	{
		NB_CORE_ASSERT(Exists(name), "Shader not found!");
		return m_Shaders[name];
	}

	bool ShaderLibrary::Exists(const std::string& name) const
	{
		return m_Shaders.contains(name);
	}

}
