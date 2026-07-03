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

	bool OpenGL410RendererAPI::IsFaceCullingEnabled() const
	{
		return glIsEnabled(GL_CULL_FACE);
	}

	void OpenGL410RendererAPI::EnableFaceCulling(bool enabled)
	{
		enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	bool OpenGL410RendererAPI::IsDepthTestEnabled() const
	{
		return glIsEnabled(GL_DEPTH_TEST);
	}

	void OpenGL410RendererAPI::EnableDepthTest(bool enabled)
	{
		enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}

	bool OpenGL410RendererAPI::IsStencilTestEnabled() const
	{
		return glIsEnabled(GL_STENCIL_TEST);
	}

	void OpenGL410RendererAPI::EnableStencilTest(bool enabled)
	{
		enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
	}

	bool OpenGL410RendererAPI::IsScissorTestEnabled() const
	{
		return glIsEnabled(GL_SCISSOR_TEST);
	}

	void OpenGL410RendererAPI::EnableScissorTest(bool enabled)
	{
		enabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	}

	void OpenGL410RendererAPI::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glScissor(x, y, width, height);
	}

	bool OpenGL410RendererAPI::IsPrimitiveRestartEnabled() const
	{
		return glIsEnabled(GL_PRIMITIVE_RESTART);
	}

	void OpenGL410RendererAPI::EnablePrimitiveRestart(bool enabled)
	{
		enabled ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
	}

	void OpenGL410RendererAPI::SetBlendingState(const BlendState& state)
	{
		if (state.enabled)
		{
			glEnable(GL_BLEND);
			glBlendEquationSeparate(GetOpenGL410BlendOperation(state.colorOperation), GetOpenGL410BlendOperation(state.alphaOperation));
			glBlendFuncSeparate(GetOpenGL410BlendFactor(state.srcColorFactor), GetOpenGL410BlendFactor(state.dstColorFactor), GetOpenGL410BlendFactor(state.srcAlphaFactor), GetOpenGL410BlendFactor(state.dstAlphaFactor));
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	RendererAPI::BlendState OpenGL410RendererAPI::GetBlendingState()
	{
		BlendState state;
		state.enabled = glIsEnabled(GL_BLEND);

		int32_t data;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &data);
		state.colorOperation = GetBlendOperationFromOpenGL410(data);

		glGetIntegerv(GL_BLEND_SRC_RGB, &data);
		state.srcColorFactor = GetBlendFactorFromOpenGL410(data);

		glGetIntegerv(GL_BLEND_DST_RGB, &data);
		state.dstColorFactor = GetBlendFactorFromOpenGL410(data);

		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &data);
		state.alphaOperation = GetBlendOperationFromOpenGL410(data);

		glGetIntegerv(GL_BLEND_SRC_ALPHA, &data);
		state.srcAlphaFactor = GetBlendFactorFromOpenGL410(data);

		glGetIntegerv(GL_BLEND_DST_ALPHA, &data);
		state.dstAlphaFactor = GetBlendFactorFromOpenGL410(data);

		return state;
	}

	uint32_t OpenGL410RendererAPI::GetMaxTextureSize() const
	{
		int32_t maxTextureSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		return maxTextureSize;
	}

	RendererAPI::PolygonMode OpenGL410RendererAPI::GetPolygonMode() const
	{
		int32_t data;
		glGetIntegerv(GL_POLYGON_MODE, &data);
		return GetPolygonModeFromOpenGL410(data);
	}

	void OpenGL410RendererAPI::SetPolygonMode(PolygonMode mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GetOpenGL410PolygonMode(mode));
	}

	RendererAPI::ClipOrigin OpenGL410RendererAPI::GetClipOrigin() const
	{
		GLint origin = 0;
		glGetIntegerv(GL_CLIP_ORIGIN, &origin);
		return GetClipOriginFromOpenGL410(origin);
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

	void OpenGL410RendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount,
		DataType dataType, uint32_t offset)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GetOpenGL410DataType(dataType), reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
	}

	void OpenGL410RendererAPI::DrawIndexedVertexOffset(const std::shared_ptr<VertexArray>& vertexArray,
		uint32_t indexCount, DataType dataType, uint32_t offset, int32_t vtxOffset)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElementsBaseVertex(GL_TRIANGLES, count, GetOpenGL410DataType(dataType), reinterpret_cast<void*>(static_cast<intptr_t>(offset)), vtxOffset);
	}

	GLenum OpenGL410RendererAPI::GetOpenGL410DataType(DataType dataType)
	{
		switch (dataType)
		{
		case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
		case DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
		case DataType::UnsignedInt: return GL_UNSIGNED_INT;
		}
	}

	GLenum OpenGL410RendererAPI::GetOpenGL410BlendFactor(BlendFactor factor)
	{
		switch (factor)
		{
		case BlendFactor::Zero: return GL_ZERO;
		case BlendFactor::One: return GL_ONE;
		case BlendFactor::SourceAlpha: return GL_SRC_ALPHA;
		case BlendFactor::OneMinusSourceAlpha: return GL_ONE_MINUS_SRC_ALPHA;
		case BlendFactor::DestinationAlpha: return GL_DST_ALPHA;
		case BlendFactor::OneMinusDestinationAlpha: return GL_ONE_MINUS_DST_ALPHA;
		}
	}

	GLenum OpenGL410RendererAPI::GetOpenGL410BlendOperation(BlendOperation operation)
	{
		switch (operation)
		{
		case BlendOperation::Add: return GL_FUNC_ADD;
		case BlendOperation::Subtract: return GL_FUNC_SUBTRACT;
		case BlendOperation::ReverseSubtract: return GL_FUNC_REVERSE_SUBTRACT;
		case BlendOperation::Minimum: return GL_MIN;
		case BlendOperation::Maximum: return GL_MAX;
		}
	}

	GLenum OpenGL410RendererAPI::GetOpenGL410PolygonMode(PolygonMode mode)
	{
		switch (mode)
		{
		case PolygonMode::Point: return GL_POINT;
		case PolygonMode::Line: return GL_LINE;
		case PolygonMode::Fill: return GL_FILL;
		}
	}

	RendererAPI::BlendFactor OpenGL410RendererAPI::GetBlendFactorFromOpenGL410(GLenum factor)
	{
		switch (factor)
		{
		case GL_ZERO: return BlendFactor::Zero;
		case GL_ONE: return BlendFactor::One;
		case GL_SRC_ALPHA: return BlendFactor::SourceAlpha;
		case GL_ONE_MINUS_SRC_ALPHA: return BlendFactor::OneMinusSourceAlpha;
		case GL_DST_ALPHA: return BlendFactor::DestinationAlpha;
		case GL_ONE_MINUS_DST_ALPHA: return BlendFactor::OneMinusDestinationAlpha;
		}
	}

	RendererAPI::BlendOperation OpenGL410RendererAPI::GetBlendOperationFromOpenGL410(GLenum operation)
	{
		switch (operation)
		{
		case GL_FUNC_ADD: return BlendOperation::Add;
		case GL_FUNC_SUBTRACT: return BlendOperation::Subtract;
		case GL_FUNC_REVERSE_SUBTRACT: return BlendOperation::ReverseSubtract;
		case GL_MIN: return BlendOperation::Minimum;
		case GL_MAX: return BlendOperation::Maximum;
		}
	}

	RendererAPI::ClipOrigin OpenGL410RendererAPI::GetClipOriginFromOpenGL410(GLint origin)
	{
		switch (origin)
		{
		case GL_LOWER_LEFT: return ClipOrigin::LowerLeft;
		case GL_UPPER_LEFT: return ClipOrigin::UpperLeft;
		default: return ClipOrigin::LowerLeft;
		}
	}

	RendererAPI::PolygonMode OpenGL410RendererAPI::GetPolygonModeFromOpenGL410(GLenum mode)
	{
		switch (mode)
		{
		case GL_POINT: return PolygonMode::Point;
		case GL_LINE: return PolygonMode::Line;
		case GL_FILL: return PolygonMode::Fill;
		default: return PolygonMode::Fill;
		}
	}
}
