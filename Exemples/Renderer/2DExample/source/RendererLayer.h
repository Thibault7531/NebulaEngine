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

private:
    Nebula::Graphics::ShaderLibrary m_ShaderLibrary;

    Nebula::Ref<Nebula::Graphics::VertexArray> m_VAO;
    Nebula::Ref<Nebula::Graphics::UniformBuffer> m_ColorUBO;

    Nebula::Ref<Nebula::Graphics::Texture2D> m_Texture, m_Glass;

    Nebula::OrthographicCameraController m_CameraController;

    glm::vec3 m_Position;
    glm::vec3 m_Color {0.f, 0.f, 0.f};
};
