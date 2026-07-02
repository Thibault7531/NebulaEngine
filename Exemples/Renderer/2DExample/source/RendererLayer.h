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
    Nebula::ShaderLibrary m_ShaderLibrary;

    Nebula::Ref<Nebula::VertexArray> m_VAO;
    Nebula::Ref<Nebula::Shader> m_Shader, m_TextureShader;

    Nebula::Ref<Nebula::Texture2D> m_Texture, m_Glass;

    Nebula::OrthographicCameraController m_CameraController;

    glm::vec3 m_Position;
    glm::vec3 m_Color;
};
