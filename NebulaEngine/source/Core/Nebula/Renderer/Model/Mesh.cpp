#include "Mesh.h"

namespace Nebula
{
    Mesh::Mesh(const Ref<Graphics::VertexArray>& vertexArray)
        : m_VertexArray(vertexArray)
    {
    }

    void Mesh::Draw()
    {
        /*
        m_VertexArray->Bind();
        RenderCommand::DrawIndexed(m_VertexArray);
        m_VertexArray->Unbind();
        */
    }
}
