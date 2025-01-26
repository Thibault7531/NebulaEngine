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

    Nebula::Ref<Nebula::Texture2D> m_Craft;

    struct ProfileResult
    {
        const char* Name;
        float Time;
    };

    glm::vec4 m_SquareColor = { 0.2f, 0.3f, 0.8f, 1.0f };
};