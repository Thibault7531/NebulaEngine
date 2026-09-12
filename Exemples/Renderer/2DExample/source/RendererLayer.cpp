#include "RendererLayer.h"

#include "gtc/type_ptr.hpp"
#include "Nebula/Debug/ImGuiDebugWindow.h"

RendererLayer::RendererLayer()
    : Layer{"Renderer2D"}, m_Device{Nebula::Application::Get().GetDevice()}, m_CameraController{16.f/9.f}, m_Position{.6f, .6f, 0.f}
{
}

void RendererLayer::OnAttach()
{
    m_VAO = Nebula::Graphics::VertexArray::Create();
    m_ColorUBO = m_Device->CreateUniformBuffer({
        { Nebula::Graphics::ShaderDataType::Float4, "u_Color" }
    });
    m_RendererUBO = m_Device->CreateUniformBuffer({
        { Nebula::Graphics::ShaderDataType::Matrix4, "u_ViewProjection" },
        { Nebula::Graphics::ShaderDataType::Matrix4, "u_Transform" }
    }, 6);

    float vertices[5 * 4] = {
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f
    };

    Nebula::Ref<Nebula::Graphics::VertexBuffer> vbo = m_Device->CreateVertexBuffer(vertices, sizeof(vertices));

    Nebula::Graphics::BufferLayout layout = {
        { Nebula::Graphics::ShaderDataType::Float3, "a_Position" },
        { Nebula::Graphics::ShaderDataType::Float2, "a_TextCoord" }
    };

    vbo->SetLayout(layout);

    m_VAO->AddVertexBuffer(vbo);

    uint32_t indices[2 * 3] = {
        0, 1, 2,
        2, 3, 0
    };

    Nebula::Ref<Nebula::Graphics::IndexBuffer> ibo = m_Device->CreateIndexBuffer(indices, std::size(indices));

    m_VAO->SetIndexBuffer(ibo);

    Nebula::Graphics::VertexLayout vertexLayout = {
        { Nebula::Graphics::ShaderDataType::Float3, "a_Position" },
        { Nebula::Graphics::ShaderDataType::Float2, "a_TextCoord" }
    };
    Nebula::Graphics::BlendState blendState = {};
    blendState.enabled = true;
    blendState.srcColorFactor = Nebula::Graphics::BlendFactor::SourceAlpha;
    blendState.dstColorFactor = Nebula::Graphics::BlendFactor::OneMinusSourceAlpha;
    blendState.srcAlphaFactor = Nebula::Graphics::BlendFactor::SourceAlpha;
    blendState.dstAlphaFactor = Nebula::Graphics::BlendFactor::OneMinusSourceAlpha;

    m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/shaders/SolidColor.slang");
    m_SolidColorShader = m_ShaderLibrary.CreatePipeline("SolidColor", vertexLayout, {Nebula::Application::Get().GetMainFramebuffer()->GetSpecification(), blendState});
    m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/shaders/Texture.slang");
    m_TextureShader = m_ShaderLibrary.CreatePipeline("Texture", vertexLayout, {Nebula::Application::Get().GetMainFramebuffer()->GetSpecification(), blendState});

    m_Texture = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/textures/crafting_table_front.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Glass = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/Renderer2DExample/textures/glass.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Nearest});

    Nebula::Application::Get().GetMainFramebuffer()->SetClearColors({{.18f, .49f, .6f, 1.f}});
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

    for (uint32_t instance = 0; instance < m_RendererUBO->GetInstanceCount(); instance++) m_RendererUBO->SetData(instance, "u_ViewProjection", glm::value_ptr(m_CameraController.GetCamera().getViewProjectionMatrix()));

    const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

    m_ColorUBO->SetData(0, "u_Color", glm::value_ptr(m_Color));

    Nebula::Ref<Nebula::Graphics::CommandBuffer> commandBuffer = m_Device->CreateCommandBuffer();
    Nebula::Ref<Nebula::Graphics::RenderPass> renderPass = commandBuffer->CreateRenderPass(Nebula::Application::Get().GetMainFramebuffer(), true);
    renderPass->SetShaderPipeline(m_SolidColorShader);

    for (int i = 0; i < 4; i++)
    {
        glm::vec3 position = m_Position + glm::vec3(0.11f, 0.0f, 0.0f) * static_cast<float>(i);
        glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

        m_RendererUBO->SetData(i, "u_Transform", glm::value_ptr(transform));

        renderPass->SetUniformBlock("ColorUBO", m_ColorUBO, 0)
            .SetUniformBlock("RendererUBO", m_RendererUBO, i)
            .DrawArrayIndexed(m_VAO);
    }

    m_RendererUBO->SetData(4, "u_Transform", glm::value_ptr(glm::mat4(1.0f)));

    renderPass->SetShaderPipeline(m_TextureShader)
        .SetUniformBlock("RendererUBO", m_RendererUBO, 4)
        .BindTextureToSlot(m_Texture)
        .DrawArrayIndexed(m_VAO);

    m_RendererUBO->SetData(5, "u_Transform", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, -0.25f, 0.0f))));

    renderPass->SetShaderPipeline(m_TextureShader)
        .SetUniformBlock("RendererUBO", m_RendererUBO, 5)
        .BindTextureToSlot(m_Glass)
        .DrawArrayIndexed(m_VAO);

    renderPass->EndRenderPass();
    commandBuffer->Commit();
}

void RendererLayer::OnImGuiRender()
{
    if (m_ShowImGuiMenuBar)
    {
        ImGui::BeginMainMenuBar();
        {
            ImGui::BeginMenu("Nebula Engine", false);
            if (ImGui::BeginMenu("Debug"))
            {
                if (ImGui::MenuItem("Show Debug Window", 0, m_ShowImGuiDebugWindow)) m_ShowImGuiDebugWindow = !m_ShowImGuiDebugWindow;
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }

    if (m_ShowImGuiDebugWindow) Nebula::Debug::ShowDebugWindow();

    ImGui::Begin("Settings");

    ImGui::ColorEdit4("Square color", glm::value_ptr(m_Color));

    ImGui::End();
}

void RendererLayer::OnEvent(Nebula::Platform::Event& event)
{
    m_CameraController.OnEvent(event);

    Nebula::Platform::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Nebula::Platform::KeyPressedEvent>(NB_BIND_EVENT_FN(OnKeyPressedEvent));
}

bool RendererLayer::OnKeyPressedEvent(Nebula::Platform::KeyPressedEvent& event)
{
    if (event.GetKeyCode() == KeyboardKey::LeftAlt) m_ShowImGuiMenuBar = !m_ShowImGuiMenuBar;
    return false;
}
