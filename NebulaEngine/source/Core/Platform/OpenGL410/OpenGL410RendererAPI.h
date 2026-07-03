#pragma once

#include "Nebula/Renderer/Core/RendererAPI.h"

#include <glad/gl.h>

namespace Nebula {

	class OpenGL410RendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void Log() override;

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
		virtual void SetBlendingState(const BlendState& state) override;
		virtual BlendState GetBlendingState() override;
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

	private:
		static GLenum GetOpenGL410DataType(DataType dataType);
		static GLenum GetOpenGL410BlendFactor(BlendFactor factor);
		static GLenum GetOpenGL410BlendOperation(BlendOperation operation);
		static GLenum GetOpenGL410PolygonMode(PolygonMode mode);

		static BlendFactor GetBlendFactorFromOpenGL410(GLenum factor);
		static BlendOperation GetBlendOperationFromOpenGL410(GLenum operation);
		static ClipOrigin GetClipOriginFromOpenGL410(GLint origin);
		static PolygonMode GetPolygonModeFromOpenGL410(GLenum mode);
	};

}
