#include "Sandbox2D.h"

#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(16.0f/9.0f)
{
}

void Sandbox2D::OnAttach()
{
    m_VertexArray = Nebula::VertexArray::Create();

    float vertices[] = {
        -0.5f, 0.5f, 0.0f,
        0.5f, 0.5f, 0.0f,
        -0.5f, -0.5f, 0.0f,
        0.5f, -0.5f, 0.0f
    };

    Nebula::Ref<Nebula::VertexBuffer> vbo = Nebula::VertexBuffer::Create(vertices, sizeof(vertices));
    Nebula::BufferLayout layout = {
        { Nebula::ShaderDataType::Float3, "a_Position"}
    };
    vbo->SetLayout(layout);
    m_VertexArray->AddVertexBuffer(vbo);

    uint32_t indices[] = {
        0, 2, 1,
        1, 2, 3
    };
    Nebula::Ref<Nebula::IndexBuffer> ibo = Nebula::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t));
    m_VertexArray->SetIndexBuffer(ibo);

    m_Shader = Nebula::Shader::Create("Solid Color", "assets/shaders/SolidColorVS.glsl", "assets/shaders/SolidColorFS.glsl", Nebula::Shader::SOURCE_TYPE::NB_SHADER_FILE);
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Nebula::Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
    Nebula::RenderCommand::Clear();

    Nebula::Renderer::BeginScene(m_CameraController.GetCamera());

    m_Shader->Bind();
    std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", m_SquareColor);
    m_Shader->Unbind();

    Nebula::Renderer::Submit(m_Shader, m_VertexArray, glm::mat4(1.0f));
}

void Sandbox2D::OnImGuiRender()
{
    ImGui::Begin("Settings");
    ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
    ImGui::End();
}

void Sandbox2D::OnEvent(Nebula::Event& e)
{
    m_CameraController.OnEvent(e);
}
