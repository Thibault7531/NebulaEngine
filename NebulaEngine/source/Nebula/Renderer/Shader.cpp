#include "Shader.h"

#include "Renderer.h"
#include "Platform/OpenGL/OpenGLShader.h"

namespace Nebula {

	Shader* Shader::Create(const std::string& vertexSrc, const std::string& fragmentSrc, SOURCE_TYPE sourceType)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLShader(vertexSrc, fragmentSrc, sourceType);
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}

	Shader* Shader::Create(const std::string& filepath)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None: NB_CORE_ASSERT(false, "RendererAPI::None is not supported"); return nullptr;
			case RendererAPI::API::OpenGL: return new OpenGLShader(filepath);
		}

		NB_CORE_ASSERT(false, "Unknown RendererAPI");
		return nullptr;
	}
}
