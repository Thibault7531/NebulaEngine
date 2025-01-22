#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"
#include "glm/gtc/matrix_transform.hpp"

namespace Nebula
{

    struct Renderer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> FlatColorShader;
    };

    static Renderer2DStorage* s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DStorage;

        s_Data->QuadVertexArray = VertexArray::Create();

        float vertices[] = {
            -0.5f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.0f,
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f
        };

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
        vertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"}
        });
        s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[] = {
            0, 2, 1,
            1, 2, 3
        };
        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

        s_Data->FlatColorShader = Shader::Create("Solid Color", "assets/shaders/SolidColorVS.glsl", "assets/shaders/SolidColorFS.glsl", Nebula::Shader::SOURCE_TYPE::NB_SHADER_FILE);
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetMat4("u_ViewProjection", camera.getViewProjectionMatrix());
        s_Data->FlatColorShader->Unbind();
    }

    void Renderer2D::EndScene()
    {
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({position.x, position.y, 0.0f}, rotation, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        s_Data->FlatColorShader->Bind();
        s_Data->FlatColorShader->SetMat4("u_Transform", transform);
        s_Data->FlatColorShader->SetFloat3("u_Color", color);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
        s_Data->QuadVertexArray->Unbind();

        s_Data->FlatColorShader->Unbind();
    }

}
