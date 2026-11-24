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
    Nebula::Ref<Nebula::Graphics::Device> m_Device;
    Nebula::Graphics::ShaderModuleLibrary m_ShaderLibrary;
    Nebula::Ref<Nebula::Graphics::ShaderPipeline> m_SolidColorShader;
    Nebula::Ref<Nebula::Graphics::ShaderPipeline> m_TextureShader;

    Nebula::Ref<Nebula::Graphics::VertexArray> m_VAO;
    Nebula::Ref<Nebula::Graphics::UniformBuffer> m_RendererUBO;
    Nebula::Ref<Nebula::Graphics::UniformBuffer> m_ColorUBO;

    Nebula::Ref<Nebula::Graphics::Texture2D> m_Texture, m_Glass;

    std::vector<Nebula::Ref<Nebula::Graphics::CommandBuffer>> m_CommandBuffers;

    Nebula::OrthographicCameraController m_CameraController;

    glm::vec3 m_Position;
    glm::vec4 m_Color {0.f, 0.f, 0.f, 1.f};

    bool m_ShowImGuiMenuBar = false;
    bool m_ShowImGuiDebugWindow = false;
};
