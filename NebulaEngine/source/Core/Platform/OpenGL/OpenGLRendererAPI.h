#pragma once

#include "../../Nebula/Graphics/Core/RendererAPI.h"

#include <glad/gl.h>

namespace Nebula {

	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Log() override {}

		virtual bool IsFaceCullingEnabled() const override;
		virtual void EnableFaceCulling(bool enabled) override;
		virtual bool IsDepthTestEnabled() const override;
		virtual void EnableDepthTest(bool enabled) override;
		virtual bool IsStencilTestEnabled() const override;
		virtual void EnableStencilTest(bool enabled) override;
		virtual bool IsScissorTestEnabled() const override;
		virtual void EnableScissorTest(bool enabled) override;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;
		virtual bool IsPrimitiveRestartEnabled() const override;
		virtual void EnablePrimitiveRestart(bool enabled) override;
		virtual BlendState GetBlendingState() override;
		virtual void SetBlendingState(const BlendState& state) override;
		virtual uint32_t GetMaxTextureSize() const override;
		virtual PolygonMode GetPolygonMode() const override;
		virtual void SetPolygonMode(PolygonMode mode) override;
		virtual ClipOrigin GetClipOrigin() const override;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) override;

		virtual void SetClearColor(const glm::vec4& color) override;
		virtual void Clear() override;

		virtual void DrawIndexed(const Ref<VertexArray>& vertexArray, uint32_t indexCount = 0) override;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, DataType dataType, uint32_t offset) override;
		virtual void DrawIndexedVertexOffset(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, DataType dataType, uint32_t offset, int32_t vtxOffset) override;

		static void GLAPIENTRY MessageCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam);

	private:
		static GLenum GetOpenGL460DataType(DataType dataType);
		static GLenum GetOpenGL460BlendFactor(BlendFactor factor);
		static GLenum GetOpenGL460BlendOperation(BlendOperation operation);
		static GLenum GetOpenGL460PolygonMode(PolygonMode mode);

		static BlendFactor GetBlendFactorFromOpenGL460(GLenum factor);
		static BlendOperation GetBlendOperationFromOpenGL460(GLenum operation);
		static PolygonMode GetPolygonModeFromOpenGL460(GLenum mode);
		static ClipOrigin GetClipOriginFromOpenGL460(GLint origin);
	};

}
