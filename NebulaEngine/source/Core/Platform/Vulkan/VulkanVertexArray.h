#pragma once

#include <vector>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"

namespace Nebula::Graphics
{
    class VulkanVertexArray : public VertexArray
    {
    public:
        VulkanVertexArray() = default;
        ~VulkanVertexArray() override = default;

        virtual void AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer) override { m_VertexBuffers.push_back(vertexBuffer); }
        virtual void SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer) override { m_IndexBuffer = indexBuffer; }

        virtual const std::vector<Ref<VertexBuffer>>& GetVertexBuffers() const override { return m_VertexBuffers; }
        virtual const Ref<IndexBuffer>& GetIndexBuffer() const override { return m_IndexBuffer; }

    private:
        std::vector<Ref<VertexBuffer>> m_VertexBuffers;
        Ref<IndexBuffer> m_IndexBuffer;
    };
}
