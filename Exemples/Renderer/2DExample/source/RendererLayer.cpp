#include "RendererLayer.h"

#include "gtc/type_ptr.hpp"

RendererLayer::RendererLayer()
    : Layer{"Renderer2D"}, m_CameraController{16.f/9.f}, m_Position{.6f, .6f, 0.f}, m_Color{0.f, 0.f, 0.f}
{
}

void RendererLayer::OnAttach()
{
    m_VAO = Nebula::VertexArray::Create();
    m_ColorUBO = Nebula::UniformBuffer::Create({
        {Nebula::ShaderDataType::Float3, "u_Color"}
    });

    float vertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };

    Nebula::Ref<Nebula::VertexBuffer> vbo = Nebula::VertexBuffer::Create(vertices, sizeof(vertices));

    Nebula::BufferLayout layout = {
        { Nebula::ShaderDataType::Float3, "a_Position" },
        { Nebula::ShaderDataType::Float2, "a_TextCoord"}
    };

    vbo->SetLayout(layout);

    m_VAO->AddVertexBuffer(vbo);

    uint32_t indices[2 * 3] = {
        0, 1, 2,
        2, 3, 0
    };

    Nebula::Ref<Nebula::IndexBuffer> ibo = Nebula::IndexBuffer::Create(indices, std::size(indices));

    m_VAO->SetIndexBuffer(ibo);

    m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/shaders/SolidColor.slang");
    m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/shaders/Texture.slang");

    m_Texture = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/textures/crafting_table_front.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Glass = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/textures/glass.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
}

void RendererLayer::OnDetach()
{
}

void RendererLayer::OnUpdate(Nebula::Timestep ts)
{
    m_CameraController.OnUpdate(ts);

    if (Nebula::Platform::Input::IsKeyPressed(KeyboardKey::UpArrow)) {
        m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * static_cast<float>(ts);
    }
    if (Nebula::Platform::Input::IsKeyPressed(KeyboardKey::DownArrow)) {
        m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * static_cast<float>(ts);
    }
    if (Nebula::Platform::Input::IsKeyPressed(KeyboardKey::RightArrow)) {
        m_Position += glm::vec3(1.0f, 0.0f, 0.0f) * static_cast<float>(ts);
    }
    if (Nebula::Platform::Input::IsKeyPressed(KeyboardKey::LeftArrow)) {
        m_Position -= glm::vec3(1.0f, 0.0f, 0.0f) * static_cast<float>(ts);
    }

    Nebula::RenderCommand::SetClearColor({0.18f, 0.49f, 0.6f, 1.0f});
    Nebula::RenderCommand::Clear();

    Nebula::Renderer::BeginScene(m_CameraController.GetCamera());

    const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    Nebula::Ref<Nebula::Shader> solidColorShader = m_ShaderLibrary.Get("SolidColor");
    solidColorShader->Bind();
    m_ColorUBO->SetData(0, glm::value_ptr(m_Color));
    solidColorShader->SetUniformBlock("ColorUBO", m_ColorUBO);
    solidColorShader->Unbind();

    for (int i = 0; i < 4; i++)
    {
        glm::vec3 position = m_Position + glm::vec3(0.11f, 0.0f, 0.0f) * static_cast<float>(i);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

        Nebula::Renderer::Submit(solidColorShader, m_VAO, transform);
    }

    m_Texture->Bind(0);
    m_Glass->Bind(1);

    m_ShaderLibrary.Get("Texture")->Bind();
    m_ShaderLibrary.Get("Texture")->SetInt("u_Texture", 0);
    Nebula::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_VAO, glm::mat4(1.0f));

    m_ShaderLibrary.Get("Texture")->Bind();
    m_ShaderLibrary.Get("Texture")->SetInt("u_Texture", 1);
    Nebula::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, -0.25f, 0.0f)));

    Nebula::Renderer::EndScene();
}

void RendererLayer::OnImGuiRender()
{
    ImGui::Begin("Settings");

    ImGui::ColorEdit3("Square color", glm::value_ptr(m_Color));

    ImGui::End();
}

void RendererLayer::OnEvent(Nebula::Platform::Event& event)
{
    m_CameraController.OnEvent(event);
}
