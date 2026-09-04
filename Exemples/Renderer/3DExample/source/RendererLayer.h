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

public:
    bool OnKeyPressedEvent(Nebula::Platform::KeyPressedEvent& event);

private:
    Nebula::Ref<Nebula::Graphics::Device> m_Device;

    Nebula::Ref<Nebula::Graphics::VertexArray> m_VAO;
    Nebula::Ref<Nebula::Graphics::UniformBuffer> m_RendererUBO;

    Nebula::Graphics::ShaderLibrary m_ShaderLibrary;
    Nebula::Ref<Nebula::Graphics::ShaderPipeline> m_TextureShader;
    Nebula::Ref<Nebula::Graphics::ShaderPipeline> m_ModelShader;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Texture;
    Nebula::Ref<Nebula::Graphics::Texture2D> m_Glass;

    Nebula::Ref<Nebula::Model> m_Backpack;

    Nebula::PerspectiveCameraController m_CameraController;

    bool m_ShowImGuiMenuBar = false;
    bool m_ShowImGuiDebugWindow = false;
};