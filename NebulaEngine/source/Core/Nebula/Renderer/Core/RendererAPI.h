#pragma once

#include <memory>
#include <glm/glm.hpp>

#include "Nebula/Renderer/Buffers/VertexArray.h"
#include "Platform/Nebula/CoreSystems/Rendering/GraphicsAPI.h"

namespace Nebula {

	class RendererAPI
	{
	public:
		enum class DataType
		{
			UnsignedByte,
			UnsignedShort,
			UnsignedInt
		};

		enum class PolygonMode
		{
			Point,
			Line,
			Fill
		};

		enum class ClipOrigin
		{
			UpperLeft,
			LowerLeft,
		};

		enum class BlendFactor
		{
			Zero,
			One,
			SourceAlpha,
			OneMinusSourceAlpha,
			DestinationAlpha,
			OneMinusDestinationAlpha
		};

		enum class BlendOperation
		{
			Add,
			Subtract,
			ReverseSubtract,
			Minimum,
			Maximum
		};

		struct BlendState
		{
			bool enabled;

			BlendFactor srcColorFactor = BlendFactor::One;
			BlendFactor dstColorFactor = BlendFactor::Zero;
			BlendOperation colorOperation = BlendOperation::Add;

			BlendFactor srcAlphaFactor = BlendFactor::One;
			BlendFactor dstAlphaFactor = BlendFactor::Zero;
			BlendOperation alphaOperation = BlendOperation::Add;
		};

	public:
		virtual ~RendererAPI() = default;

		virtual void Init() = 0;
		virtual void Log() = 0;

		virtual bool IsFaceCullingEnabled() const = 0;
		virtual void EnableFaceCulling(bool enabled) = 0;
		virtual bool IsDepthTestEnabled() const = 0;
		virtual void EnableDepthTest(bool enabled) = 0;
		virtual bool IsStencilTestEnabled() const = 0;
		virtual void EnableStencilTest(bool enabled) = 0;
		virtual bool IsScissorTestEnabled() const = 0;
		virtual void EnableScissorTest(bool enabled) = 0;
		virtual void SetScissor(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;
		virtual bool IsPrimitiveRestartEnabled() const = 0;
		virtual void EnablePrimitiveRestart(bool enabled) = 0;
		virtual void SetBlendingState(const BlendState& state) = 0;
		virtual BlendState GetBlendingState() = 0;
		virtual uint32_t GetMaxTextureSize() const = 0;
		virtual PolygonMode GetPolygonMode() const = 0;
		virtual void SetPolygonMode(PolygonMode mode) = 0;
		virtual ClipOrigin GetClipOrigin() const = 0;

		virtual void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height) = 0;

		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount = 0) = 0;
		virtual void DrawIndexed(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, DataType dataType, uint32_t offset) = 0;
		virtual void DrawIndexedVertexOffset(const std::shared_ptr<VertexArray>& vertexArray, uint32_t indexCount, DataType dataType, uint32_t offset, int32_t vtxOffset) = 0;

		inline static Platform::GraphicsAPI GetAPI() { return s_API; }

	private:
		static Platform::GraphicsAPI s_API;

		friend class RenderCommand;
	};

}
