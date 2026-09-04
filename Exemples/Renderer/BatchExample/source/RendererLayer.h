#pragma once

#include <Nebula.h>

class RendererLayer : public Nebula::Layer
{
public:
    RendererLayer();
    ~RendererLayer() override = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Nebula::Timestep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Nebula::Platform::Event& event) override;

public:
    bool OnKeyPressedEvent(Nebula::Platform::KeyPressedEvent& event);

private:
    Nebula::OrthographicCameraController m_CameraController;

    Nebula::Ref<Nebula::Graphics::Device> m_Device;

    Nebula::Ref<Nebula::Graphics::Texture2D> m_Stone;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Dirt;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Grass;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Craft;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Deepslate;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Bedrock;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Diamond;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Iron;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Coal;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Rainbow;

    std::array<std::array<Nebula::Ref<Nebula::Graphics::Texture2D>, 32>, 32> m_World;

    struct ProfileResult
    {
        const char* Name;
        float Time;
    };

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };

    bool m_ShowImGuiMenuBar = false;
    bool m_ShowImGuiDebugWindow = false;
};
