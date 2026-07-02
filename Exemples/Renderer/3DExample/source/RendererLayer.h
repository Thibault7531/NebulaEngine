#pragma once

#include <Nebula.h>

class RendererLayer : public Nebula::Layer
{
public:
    RendererLayer();
    virtual ~RendererLayer() = default;

    virtual void OnAttach() override;
    virtual void OnDetach() override;

    virtual void OnUpdate(Nebula::Timestep ts) override;
    virtual void OnImGuiRender() override;
    virtual void OnEvent(Nebula::Platform::Event& event) override;

private:
    Nebula::Ref<Nebula::VertexArray> m_VAO;

    Nebula::ShaderLibrary m_ShaderLibrary;
    Nebula::Ref<Nebula::Texture2D> m_Texture;
    Nebula::Ref<Nebula::Texture2D> m_Glass;

    Nebula::Ref<Nebula::Model> m_Backpack;

    Nebula::PerspectiveCameraController m_CameraController;
};