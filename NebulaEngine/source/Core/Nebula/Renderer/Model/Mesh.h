#pragma once

#include "Nebula/Graphics/Buffers/Buffer.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"

namespace Nebula
{

    class Mesh
    {
    public:
        Mesh(const Ref<Graphics::VertexArray>& vertexArray);
        ~Mesh() = default;

        Ref<Graphics::VertexArray> GetVertexArray() const { return m_VertexArray; }

    private:
        Ref<Graphics::VertexArray> m_VertexArray;
    };

}
