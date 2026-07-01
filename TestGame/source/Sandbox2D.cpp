#include "Sandbox2D.h"

#include <Nebula/Debug/Instrumentor.h>

#include "glm/gtc/type_ptr.hpp"
#include "Platform/OpenGL/OpenGLShader.h"

Sandbox2D::Sandbox2D()
    : Layer("Sandbox2D"), m_CameraController(16.0f/9.0f)
{
}

void Sandbox2D::OnAttach()
{
    m_Dirt = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/dirt.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Grass = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/grass_block_side.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Stone = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/stone.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Craft = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/crafting_table_front.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Deepslate = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/deepslate.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Bedrock = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() /"assets/TestGame/textures/bedrock.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Diamond = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/diamond_ore.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Iron = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/iron_ore.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Coal = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/coal_ore.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
    m_Rainbow = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/TestGame/textures/furnace_front.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});

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
}

void Sandbox2D::OnDetach()
{
}

void Sandbox2D::OnUpdate(Nebula::Timestep ts)
{
    NB_PROFILE_FUNCTION();

    m_CameraController.OnUpdate(ts);

    Nebula::Renderer2D::ResetStats();
    {
        NB_PROFILE_SCOPE("Viewport clear");
        Nebula::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1.0f });
        Nebula::RenderCommand::Clear();
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

        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (int x = 0; x < 100; x++) {
            for (int y = 0; y < 100; y++) {
                Nebula::Renderer2D::DrawQuad({ .position = {x, y, 0.0f}, .texture = m_Dirt});
            }
        }
        Nebula::Renderer2D::EndScene();

        /*
        Nebula::Renderer2D::BeginScene(m_CameraController.GetCamera());
        for (int y = 0; y < 32; y++) {
            for (int x = 0; x < 32; x++) {
                if (m_World[y][x] == m_Rainbow) Nebula::Renderer2D::DrawQuad( { .position = {x, y, 0.0f}, .color = m_SquareColor } );
                else if (m_World[y][x] != nullptr) Nebula::Renderer2D::DrawQuad( { .position = {x, y, 0.0f}, .texture = m_World[y][x] } );
            }
        }
        Nebula::Renderer2D::EndScene();
        */
    }
}

void Sandbox2D::OnImGuiRender()
{
    NB_PROFILE_FUNCTION();


}

void Sandbox2D::OnEvent(Nebula::Platform::Event& e)
{
    NB_PROFILE_FUNCTION();

    m_CameraController.OnEvent(e);
}
