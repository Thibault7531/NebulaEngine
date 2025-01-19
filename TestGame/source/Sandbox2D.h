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

    Nebula::Ref<Nebula::VertexArray> m_VertexArray;
    Nebula::Ref<Nebula::Shader> m_Shader;

    glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};