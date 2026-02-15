#include "Mesh.h"

#include "Nebula/Renderer/Core/RenderCommand.h"

namespace Nebula
{
    Mesh::Mesh(const Ref<VertexArray>& vertexArray)
        : m_VertexArray(vertexArray)
    {
    }

    void Mesh::Draw()
    {
        m_VertexArray->Bind();
        RenderCommand::DrawIndexed(m_VertexArray);
        m_VertexArray->Unbind();
    }
}
