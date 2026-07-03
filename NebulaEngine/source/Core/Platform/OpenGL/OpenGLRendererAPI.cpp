#include "OpenGLRendererAPI.h"

#include <glad/gl.h>

namespace Nebula {
	void OpenGLRendererAPI::Init()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("OpenGL Info:");
		NB_CORE_INFO("  Vendor: {0}", (const char*)glGetString(GL_VENDOR));
		NB_CORE_INFO("  Renderer: {0}", (const char*)glGetString(GL_RENDERER));
		NB_CORE_INFO("  Version: {0}", (const char*)glGetString(GL_VERSION));

		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback(MessageCallback, nullptr);
	}

	bool OpenGLRendererAPI::IsFaceCullingEnabled() const
	{
		return glIsEnabled(GL_CULL_FACE);
	}

	void OpenGLRendererAPI::EnableFaceCulling(bool enabled)
	{
		enabled ? glEnable(GL_CULL_FACE) : glDisable(GL_CULL_FACE);
	}

	bool OpenGLRendererAPI::IsDepthTestEnabled() const
	{
		return glIsEnabled(GL_DEPTH_TEST);
	}

	void OpenGLRendererAPI::EnableDepthTest(bool enabled)
	{
		enabled ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}

	bool OpenGLRendererAPI::IsStencilTestEnabled() const
	{
		return glIsEnabled(GL_STENCIL_TEST);
	}

	void OpenGLRendererAPI::EnableStencilTest(bool enabled)
	{
		enabled ? glEnable(GL_STENCIL_TEST) : glDisable(GL_STENCIL_TEST);
	}

	bool OpenGLRendererAPI::IsScissorTestEnabled() const
	{
		return glIsEnabled(GL_SCISSOR_TEST);
	}

	void OpenGLRendererAPI::EnableScissorTest(bool enabled)
	{
		enabled ? glEnable(GL_SCISSOR_TEST) : glDisable(GL_SCISSOR_TEST);
	}

	void OpenGLRendererAPI::SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glScissor(x, y, width, height);
	}

	bool OpenGLRendererAPI::IsPrimitiveRestartEnabled() const
	{
		return glIsEnabled(GL_PRIMITIVE_RESTART);
	}

	void OpenGLRendererAPI::EnablePrimitiveRestart(bool enabled)
	{
		enabled ? glEnable(GL_PRIMITIVE_RESTART) : glDisable(GL_PRIMITIVE_RESTART);
	}

	RendererAPI::BlendState OpenGLRendererAPI::GetBlendingState()
	{
		BlendState state;
		state.enabled = glIsEnabled(GL_BLEND);

		int32_t data;
		glGetIntegerv(GL_BLEND_EQUATION_RGB, &data);
		state.colorOperation = GetBlendOperationFromOpenGL460(data);

		glGetIntegerv(GL_BLEND_SRC_RGB, &data);
		state.srcColorFactor = GetBlendFactorFromOpenGL460(data);

		glGetIntegerv(GL_BLEND_DST_RGB, &data);
		state.dstColorFactor = GetBlendFactorFromOpenGL460(data);

		glGetIntegerv(GL_BLEND_EQUATION_ALPHA, &data);
		state.alphaOperation = GetBlendOperationFromOpenGL460(data);

		glGetIntegerv(GL_BLEND_SRC_ALPHA, &data);
		state.srcAlphaFactor = GetBlendFactorFromOpenGL460(data);

		glGetIntegerv(GL_BLEND_DST_ALPHA, &data);
		state.dstAlphaFactor = GetBlendFactorFromOpenGL460(data);

		return state;
	}

	void OpenGLRendererAPI::SetBlendingState(const BlendState& state)
	{
		if (state.enabled)
		{
			glEnable(GL_BLEND);
			glBlendEquationSeparate(GetOpenGL460BlendOperation(state.colorOperation), GetOpenGL460BlendOperation(state.alphaOperation));
			glBlendFuncSeparate(GetOpenGL460BlendFactor(state.srcColorFactor), GetOpenGL460BlendFactor(state.dstColorFactor), GetOpenGL460BlendFactor(state.srcAlphaFactor), GetOpenGL460BlendFactor(state.dstAlphaFactor));
		}
		else
		{
			glDisable(GL_BLEND);
		}
	}

	uint32_t OpenGLRendererAPI::GetMaxTextureSize() const
	{
		int32_t maxTextureSize = 0;
		glGetIntegerv(GL_MAX_TEXTURE_SIZE, &maxTextureSize);
		return maxTextureSize;
	}

	RendererAPI::PolygonMode OpenGLRendererAPI::GetPolygonMode() const
	{
		int32_t data;
		glGetIntegerv(GL_POLYGON_MODE, &data);
		return GetPolygonModeFromOpenGL460(data);
	}

	void OpenGLRendererAPI::SetPolygonMode(PolygonMode mode)
	{
		glPolygonMode(GL_FRONT_AND_BACK, GetOpenGL460PolygonMode(mode));
	}

	RendererAPI::ClipOrigin OpenGLRendererAPI::GetClipOrigin() const
	{
		GLint origin = 0;
		glGetIntegerv(GL_CLIP_ORIGIN, &origin);
		return GetClipOriginFromOpenGL460(origin);
	}

	void OpenGLRendererAPI::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
	{
		glViewport(x, y, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexArray>& vertexArray, const uint32_t indexCount)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount,
		DataType dataType, uint32_t offset)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElements(GL_TRIANGLES, count, GetOpenGL460DataType(dataType), reinterpret_cast<void*>(static_cast<intptr_t>(offset)));
	}

	void OpenGLRendererAPI::DrawIndexedVertexOffset(const std::shared_ptr<VertexArray>& vertexArray,
		uint32_t indexCount, DataType dataType, uint32_t offset, int32_t vtxOffset)
	{
		uint32_t count = indexCount == 0 ? vertexArray->GetIndexBuffer()->GetCount() : indexCount;
		glDrawElementsBaseVertex(GL_TRIANGLES, count, GetOpenGL460DataType(dataType), reinterpret_cast<void*>(static_cast<intptr_t>(offset)), vtxOffset);
	}

	void OpenGLRendererAPI::MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length,
	                                        const GLchar *message, const void *userParam)
	{
		NB_PROFILE_FUNCTION();

		if (id == 131185) return;

		if (type == GL_DEBUG_TYPE_ERROR)
		{
			NB_CORE_ERROR("GL CALLBACK: {0} type = {1}, severity = {2}, message = {3}\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message);
			NB_CORE_ASSERT(false);
		}
		else
		{
			NB_CORE_DEBUG("GL CALLBACK: {0} type = {1}, severity = {2}, message = {3}\n", ( type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : "" ), type, severity, message);
		}
	}

	GLenum OpenGLRendererAPI::GetOpenGL460DataType(DataType dataType)
	{
		switch (dataType)
		{
		case DataType::UnsignedByte: return GL_UNSIGNED_BYTE;
		case DataType::UnsignedShort: return GL_UNSIGNED_SHORT;
		case DataType::UnsignedInt: return GL_UNSIGNED_INT;
		}
	}

	GLenum OpenGLRendererAPI::GetOpenGL460BlendFactor(BlendFactor factor)
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

	GLenum OpenGLRendererAPI::GetOpenGL460BlendOperation(BlendOperation operation)
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

	GLenum OpenGLRendererAPI::GetOpenGL460PolygonMode(PolygonMode mode)
	{
		switch (mode)
		{
		case PolygonMode::Point: return GL_POINT;
		case PolygonMode::Line: return GL_LINE;
		case PolygonMode::Fill: return GL_FILL;
		}
	}

	RendererAPI::BlendFactor OpenGLRendererAPI::GetBlendFactorFromOpenGL460(GLenum factor)
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

	RendererAPI::BlendOperation OpenGLRendererAPI::GetBlendOperationFromOpenGL460(GLenum operation)
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

	RendererAPI::PolygonMode OpenGLRendererAPI::GetPolygonModeFromOpenGL460(GLenum mode)
	{
		switch (mode)
		{
		case GL_POINT: return PolygonMode::Point;
		case GL_LINE: return PolygonMode::Line;
		case GL_FILL: return PolygonMode::Fill;
		default: return PolygonMode::Fill;
		}
	}

	RendererAPI::ClipOrigin OpenGLRendererAPI::GetClipOriginFromOpenGL460(GLint origin)
	{
		switch (origin)
		{
		case GL_LOWER_LEFT: return ClipOrigin::LowerLeft;
		case GL_UPPER_LEFT: return ClipOrigin::UpperLeft;
		default: return ClipOrigin::LowerLeft;
		}
	}
}
