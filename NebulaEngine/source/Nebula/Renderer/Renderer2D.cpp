#include "Renderer2D.h"

#include "RenderCommand.h"
#include "Shader.h"
#include "VertexArray.h"
#include "glm/gtc/matrix_transform.hpp"
#include "gtc/type_ptr.inl"

namespace Nebula
{

    struct Renderer2DStorage
    {
        Ref<VertexArray> QuadVertexArray;
        Ref<Shader> Shader2D;
        Ref<Texture2D> WhiteTexture;
    };

    static Renderer2DStorage* s_Data;

    void Renderer2D::Init()
    {
        s_Data = new Renderer2DStorage;

        s_Data->QuadVertexArray = VertexArray::Create();

        float vertices[] = {
            -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,
            0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
            0.5f, -0.5f, 0.0f, 1.0f, 0.0f
        };

        Ref<VertexBuffer> vertexBuffer = VertexBuffer::Create(vertices, sizeof(vertices));
        vertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float2, "a_TexCoord"},
        });
        s_Data->QuadVertexArray->AddVertexBuffer(vertexBuffer);

        uint32_t indices[] = {
            0, 2, 1,
            1, 2, 3
        };
        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
        s_Data->QuadVertexArray->SetIndexBuffer(indexBuffer);

        s_Data->Shader2D = Shader::Create("assets/shaders/Renderer2D.glsl");

        s_Data->WhiteTexture = Texture2D::Create(1, 1);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data->WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t));
        s_Data->WhiteTexture->Bind();
    }

    void Renderer2D::Shutdown()
    {
        delete s_Data;
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        s_Data->Shader2D->Bind();
        s_Data->Shader2D->SetMat4("u_ViewProjection", camera.getViewProjectionMatrix());
    }

    void Renderer2D::EndScene()
    {
        s_Data->Shader2D->Unbind();
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
    {
        DrawQuad({position.x, position.y, 0.0f}, rotation, size, color);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        s_Data->Shader2D->SetMat4("u_Transform", transform);
        s_Data->Shader2D->SetFloat4("u_Color", color);
        s_Data->Shader2D->SetInt("u_Texture", 0);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
        s_Data->QuadVertexArray->Unbind();
    }

    void Renderer2D::DrawQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture)
    {
        DrawQuad({position.x, position.y, 0.0f}, rotation, size, color, texture);
    }

    void Renderer2D::DrawQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color, const Ref<Texture2D>& texture)
    {
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * glm::rotate(glm::mat4(1.0f), glm::radians(rotation), {0.0f, 0.0f, 1.0f}) * glm::scale(glm::mat4(1.0f), { size.x, size.y, 1.0f });

        texture->Bind(1);
        s_Data->Shader2D->SetMat4("u_Transform", transform);
        s_Data->Shader2D->SetFloat4("u_Color", color);
        s_Data->Shader2D->SetInt("u_Texture", 1);

        s_Data->QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data->QuadVertexArray);
        s_Data->QuadVertexArray->Unbind();
    }
}
