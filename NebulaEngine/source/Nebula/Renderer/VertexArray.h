#pragma once
#include "Buffer.h"

namespace Nebula {
	/**
	 * Class representing a vertex array.
	 */
	class VertexArray
	{
	public:
		virtual ~VertexArray() {}

		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		/**
		 * Adds a vertex buffer to the vertex array.
		 * @param vertexBuffer Vertex buffer to be added to the vertex array
		 */
		virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) = 0;
		/**
		 * Sets the index buffer of the vertex array.
		 * @param indexBuffer Index buffer to be set to the vertex array
		 */
		virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) = 0;

		/**
		 * Gets all the associated vertex buffers.
		 * @return Vector with all the vertex buffers
		 */
		virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const = 0;
		/**
		 * Gets the index buffer associated to the vertex array.
		 * @return Index buffer associated to the vertex array
		 */
		virtual const Ref<IndexBuffer>& GetIndexBuffer() const = 0;

		/**
		 * Creates a vertex array.
		 * @return Pointer to the generated vertex array.
		 */
		static VertexArray* Create();
	};

}
