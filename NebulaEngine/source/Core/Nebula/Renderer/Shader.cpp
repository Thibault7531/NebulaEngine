#include "Shader.h"

#include "Renderer.h"

#ifdef NB_PLATFORM_MACOS
#include "Platform/OpenGL410/OpenGL410Shader.h"
#else
#include "Platform/OpenGL/OpenGLShader.h"
#include "Platform/OpenGL410/OpenGL410Shader.h"
#endif

namespace Nebula {

	Ref<Shader> Shader::Create(const std::string& name, const std::string& vertexSrc, const std::string& fragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexSrc, fragmentSrc);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexSrc, fragmentSrc);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLShader>(name, vertexSrc, fragmentSrc);
#endif
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Ref<Shader> Shader::Create(const std::string& name, const std::filesystem::path& vertexPath,
		const std::filesystem::path& fragmentPath)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexPath, fragmentPath);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(name, vertexPath, fragmentPath);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLShader>(name, vertexPath, fragmentPath);
#endif
		}
	}

	Ref<Shader> Shader::Create(const std::filesystem::path& filepath)
	{
		switch (Renderer::GetAPI())
		{
#ifdef NB_PLATFORM_MACOS
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(filepath);
		case Platform::GraphicsAPI::OpenGL460: NB_CORE_ASSERT(false, "RendererAPI::OpenGL not supported on MacOS"); return nullptr;
#else
		case Platform::GraphicsAPI::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
		case Platform::GraphicsAPI::OpenGL410: return std::make_shared<OpenGL410Shader>(filepath);
		case Platform::GraphicsAPI::OpenGL460: return std::make_shared<OpenGLShader>(filepath);
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

	Ref<Shader> ShaderLibrary::Load(const std::filesystem::path& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& filepath)
	{
		auto shader = Shader::Create(filepath);
		Add(name, shader);
		return shader;
	}

	Ref<Shader> ShaderLibrary::Load(const std::string& name, const std::filesystem::path& vertexPath, const std::filesystem::path& fragmentPath)
	{
		auto shader = Shader::Create(name, vertexPath, fragmentPath);
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
