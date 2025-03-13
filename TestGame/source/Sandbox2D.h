#pragma once

#include <Nebula.h>

class Sandbox2D : public Nebula::Layer
{
public:
    Sandbox2D();
    ~Sandbox2D() override = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Nebula::Timestep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Nebula::Event& e) override;

private:
    Nebula::OrthographicCameraController m_CameraController;

    Nebula::Ref<Nebula::Texture2D> m_Stone;
    Nebula::Ref<Nebula::Texture2D> m_Dirt;
    Nebula::Ref<Nebula::Texture2D> m_Grass;
    Nebula::Ref<Nebula::Texture2D> m_Craft;
    Nebula::Ref<Nebula::Texture2D> m_Deepslate;
    Nebula::Ref<Nebula::Texture2D> m_Bedrock;
    Nebula::Ref<Nebula::Texture2D> m_Diamond;
    Nebula::Ref<Nebula::Texture2D> m_Iron;
    Nebula::Ref<Nebula::Texture2D> m_Coal;
    Nebula::Ref<Nebula::Texture2D> m_Rainbow;

    std::array<std::array<Nebula::Ref<Nebula::Texture2D>, 32>, 32> m_World;

    Nebula::Ref<Nebula::Framebuffer> m_Framebuffer;

    struct ProfileResult
    {
        const char* Name;
        float Time;
    };

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};