#pragma once

#include "Nebula/Graphics/Buffers/VertexArray.h"

namespace Nebula::Graphics
{
    class Metal3VertexArray : public VertexArray
    {
    public:
        Metal3VertexArray() = default;
        ~Metal3VertexArray() = default;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override { m_VertexBuffers.push_back(vertexBuffer); }
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override { m_IndexBuffer = indexBuffer; }

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };
}
