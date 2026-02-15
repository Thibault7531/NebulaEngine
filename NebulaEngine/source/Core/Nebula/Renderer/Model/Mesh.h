#pragma once

#include "Nebula/Renderer/Buffers/Buffer.h"
#include "Nebula/Renderer/Buffers/VertexArray.h"

namespace Nebula
{

    class Mesh
    {
    public:
        Mesh(const Ref<VertexArray>& vertexArray);
        ~Mesh() = default;

        void Draw();

    private:
        Ref<VertexArray> m_VertexArray;
    };

}
