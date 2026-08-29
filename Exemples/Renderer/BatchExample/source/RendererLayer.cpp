#include "RendererLayer.h"

RendererLayer::RendererLayer()
    : Layer{"BatchRenderer"}, m_CameraController{16.f/9.f}, m_Device{Nebula::Application::Get().GetDevice()}
{
}

void RendererLayer::OnAttach()
{
    m_Dirt = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/dirt.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Grass = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/grass_block_side.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Stone = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/stone.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Craft = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/crafting_table_front.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Deepslate = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/deepslate.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Bedrock = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() /"assets/BatchRendererExample/textures/bedrock.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Diamond = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/diamond_ore.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Iron = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/iron_ore.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Coal = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/coal_ore.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});
    m_Rainbow = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/BatchRendererExample/textures/furnace_front.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Linear, Nebula::Graphics::Texture::FilterType::Nearest});

    for (int Y = 0; Y < 32; Y++) {
        for (int X = 0; X < 32; X++) {
            if (Y == 0) m_World[Y][X] = m_Bedrock;
            else if (Y < 12) m_World[Y][X] = m_Deepslate;
            else if (Y == 12) {
                int random = std::rand();
                if (random % 4 == 0) m_World[Y][X] = m_Stone;
                else m_World[Y][X] = m_Deepslate;
            }
            else if (Y == 13) {
                int random = std::rand();
                if (random % 3 == 0) m_World[Y][X] = m_Stone;
                else m_World[Y][X] = m_Deepslate;
            }
            else if (Y == 14) {
                int random = std::rand();
                if (random % 2 == 0) m_World[Y][X] = m_Stone;
                else m_World[Y][X] = m_Deepslate;
            }
            else if (Y < 24) {
                int random = std::rand();
                if (random % 256 == 0) m_World[Y][X] = m_Rainbow;
                else if (random % 128 == 0) m_World[Y][X] = m_Diamond;
                else if (random % 12 == 0) m_World[Y][X] = m_Iron;
                else if (random % 8 == 0) m_World[Y][X] = m_Coal;
                else m_World[Y][X] = m_Stone;
            }
            else if (Y == 24) {
                int random = std::rand();
                if (random % 2 == 0) m_World[Y][X] = m_Stone;
                else m_World[Y][X] = m_Dirt;
            }
            else if (Y == 25) {
                int random = std::rand();
                if (random % 4 == 0) m_World[Y][X] = m_Stone;
                else m_World[Y][X] = m_Dirt;
            }
            else if (Y < 28) m_World[Y][X] = m_Dirt;
            else if (Y == 28) m_World[Y][X] = m_Grass;
        }
    }

    Nebula::Application::Get().GetMainFramebuffer()->SetClearColors({{ 0.1f, 0.1f, 0.1f, 1.0f }});
}

void RendererLayer::OnDetach()
{
}

void RendererLayer::OnUpdate(Nebula::Timestep ts)
{
    NB_PROFILE_FUNCTION();

    m_CameraController.OnUpdate(ts);

    Nebula::Renderer2D::ResetStats();
    {
        NB_PROFILE_SCOPE("Viewport clear");
        Nebula::Ref<Nebula::Graphics::CommandBuffer> ClearCommandBuffer = m_Device->CreateCommandBuffer();
        Nebula::Ref<Nebula::Graphics::RenderPass> ClearRenderPass = ClearCommandBuffer->CreateRenderPass(Nebula::Application::Get().GetMainFramebuffer(), true);
        ClearRenderPass->EndRenderPass();
        ClearCommandBuffer->Commit();
    }

    {
        NB_PROFILE_SCOPE("Render");

        /*
        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());

        Nebula::Renderer2D::DrawQuad( { .position = {-1.0f, 0.0f, 0.0f}, .rotation = glm::radians(45.0f), .size = {1.0f, 0.8f}, .color = {0.8f, 0.2f, 0.3f, 1.0f} } );
        Nebula::Renderer2D::DrawQuad( { .position = {0.5f, -0.5f, 0.0f}, .rotation = glm::radians(10.0f), .size = {0.5f, 0.8f}, .color = m_SquareColor } );
        Nebula::Renderer2D::DrawQuad( { .position = {1.0f, 0.5f, 0.0f}, .size = {1.0f, 0.8f}, .texture = m_Craft, .color = {0.8f, 1.0f, 1.0f, 1.0f}, .tilingFactor = 10.0f } );

        Nebula::Renderer2D::EndScene();
        */

        /*
        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                Nebula::Renderer2D::DrawQuad({ .position = {x, y, 0.0f}, .texture = m_Dirt});
            }
        }
        Nebula::Renderer2D::EndScene();
        */


        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                if (m_World[y][x] == m_Rainbow) Nebula::Renderer2D::DrawQuad( { .position = {x, y, 0.0f}, .color = m_SquareColor } );
                else if (m_World[y][x] != nullptr) Nebula::Renderer2D::DrawQuad( { .position = {x, y, 0.0f}, .texture = m_World[y][x] } );
            }
        }
        Nebula::Renderer2D::EndScene();

    }
}

void RendererLayer::OnImGuiRender()
{
}

void RendererLayer::OnEvent(Nebula::Platform::Event& event)
{
    m_CameraController.OnEvent(event);
}
