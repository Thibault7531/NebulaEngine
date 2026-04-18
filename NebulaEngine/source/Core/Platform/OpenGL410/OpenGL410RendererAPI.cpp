#include "OpenGL410RendererAPI.h"

#include <glad/gl.h>

namespace Nebula {
	void OpenGL410RendererAPI::Init()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("OpenGL Info:");
		NB_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		NB_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		NB_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glEnable(GL_DEPTH_TEST);

		glEnable(GL_CULL_FACE);

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
	}

	void OpenGL410RendererAPI::Log()
	{
#ifdef NB_PLATFORM_WINDOWS
		GLint len;
		glGetIntegerv(GL_DEBUG_NEXT_LOGGED_MESSAGE_LENGTH_ARB, &len);

		auto message = static_cast<GLchar*>(malloc(sizeof(GLchar) * len));
		GLenum source;
		GLenum type;
		GLuint id;
		GLenum severity;
		GLsizei length;
		glGetDebugMessageLogARB(1, len, &source, &type, &id, &severity, &length, message);
		if (type == GL_DEBUG_TYPE_ERROR_ARB) NB_CORE_ERROR("OpenGL Error: {0}", message);
#endif
	}

	void OpenGL410RendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGL410RendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGL410RendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGL410RendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}
}
