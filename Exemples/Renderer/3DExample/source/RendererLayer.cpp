#include "RendererLayer.h"

#include "gtc/type_ptr.hpp"
#include "Nebula/Debug/ImGuiDebugWindow.h"

RendererLayer::RendererLayer()
    : m_Device{Nebula::Application::Get().GetDevice()}, m_CameraController{16.f/9.f, glm::radians(60.f)}
{
}

void RendererLayer::OnAttach()
{
    m_VAO = Nebula::Graphics::VertexArray::Create();

	float vertices[] = {
		0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 1.0f, 0.0f,

		1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
		1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

		0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 1.0f, 1.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f, 1.0f, 0.0f
	};

	Nebula::Ref<Nebula::Graphics::VertexBuffer> vbo = m_Device->CreateVertexBuffer(vertices, sizeof(vertices));

	Nebula::Graphics::BufferLayout layout = {
		{ Nebula::Graphics::ShaderDataType::Float3, "a_Position" },
		{ Nebula::Graphics::ShaderDataType::Float2, "a_TextCoord"}
	};

	vbo->SetLayout(layout);

	m_VAO->AddVertexBuffer(vbo);

	uint32_t indices[] = {
		0, 3, 2,
		0, 1, 3,

		5, 6, 7,
		5, 4, 6,

		9, 10, 11,
		9, 8, 10,

		14, 13, 12,
		14, 15, 13,

		18, 17, 16,
		18, 19, 17,

		22, 21, 20,
		22, 23, 21
	};

	Nebula::Ref<Nebula::Graphics::IndexBuffer> ibo = m_Device->CreateIndexBuffer(indices, std::size(indices));

	m_VAO->SetIndexBuffer(ibo);

	Nebula::Graphics::VertexLayout textureVertexLayout = {
		{ Nebula::Graphics::ShaderDataType::Float3, "a_Position" },
		{ Nebula::Graphics::ShaderDataType::Float2, "a_TextCoord" }
	};
	Nebula::Graphics::VertexLayout modelVertexLayout = {
		{ Nebula::Graphics::ShaderDataType::Float3, "a_Position" },
		{ Nebula::Graphics::ShaderDataType::Float3, "a_Normal" }
	};

	m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/shaders/Texture.slang");
	m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/shaders/Model.slang");
	Nebula::Graphics::BlendState blendState = {};
	blendState.enabled = true;
	blendState.srcColorFactor = Nebula::Graphics::BlendFactor::SourceAlpha;
	blendState.dstColorFactor = Nebula::Graphics::BlendFactor::OneMinusSourceAlpha;
	blendState.srcAlphaFactor = Nebula::Graphics::BlendFactor::SourceAlpha;
	blendState.dstAlphaFactor = Nebula::Graphics::BlendFactor::OneMinusSourceAlpha;
	m_TextureShader = m_ShaderLibrary.CreatePipeline("Texture", textureVertexLayout, {.framebufferSpec = Nebula::Application::Get().GetMainFramebuffer()->GetSpecification(), .blendState = blendState});
	m_ModelShader = m_ShaderLibrary.CreatePipeline("Model", modelVertexLayout, {.framebufferSpec = Nebula::Application::Get().GetMainFramebuffer()->GetSpecification()});

	m_Texture = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/textures/crafting_table_front.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Nearest});
	m_Glass = m_Device->CreateTexture2D(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/textures/glass.png", {Nebula::Graphics::Texture::InternalFormat::RGBA8, Nebula::Graphics::Texture::FilterType::Nearest});

	m_Backpack = Nebula::Model::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/models/backpack.fbx").string());

	m_CameraController.GetCamera().setPosition({0.f, 0.f, 3.f});

	Nebula::Application::Get().GetMainFramebuffer()->SetClearColors({{0.18f, 0.49f, 0.6f, 1.f}});
	m_RendererUBO = m_Device->CreateUniformBuffer({
		{ Nebula::Graphics::ShaderDataType::Matrix4, "u_ViewProjection" },
		{ Nebula::Graphics::ShaderDataType::Matrix4, "u_Transform" },
	}, 4);
}

void RendererLayer::OnDetach()
{
	m_TextureShader.reset();
	m_ModelShader.reset();
}

void RendererLayer::OnUpdate(Nebula::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	for (uint32_t instance = 0; instance < m_RendererUBO->GetInstanceCount(); instance++) m_RendererUBO->SetData(instance, "u_ViewProjection", glm::value_ptr(m_CameraController.GetCamera().getViewProjectionMatrix()));

	m_RendererUBO->SetData(0, "u_Transform", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f))));

	Nebula::Ref<Nebula::Graphics::CommandBuffer> commandBuffer = m_Device->CreateCommandBuffer();
	Nebula::Ref<Nebula::Graphics::RenderPass> renderPass = commandBuffer->CreateRenderPass(Nebula::Application::Get().GetMainFramebuffer(), true);

	renderPass->SetShaderPipeline(m_TextureShader)
		.SetUniformBlock("RendererUBO", m_RendererUBO, 0)
		.SetCullMode(Nebula::Graphics::CullMode::Front)
		.SetDepthStencilState({.depthCompareFunction = Nebula::Graphics::DepthCompareFunction::Less, .depthWrite = true})
		.BindTextureToSlot(m_Texture)
		.DrawArrayIndexed(m_VAO);

	m_RendererUBO->SetData(1, "u_Transform", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(-0.5f, -0.5f, -10.5f))));

	renderPass->SetShaderPipeline(m_TextureShader)
		.SetUniformBlock("RendererUBO", m_RendererUBO, 1)
		.SetCullMode(Nebula::Graphics::CullMode::Front)
		.SetDepthStencilState({.depthCompareFunction = Nebula::Graphics::DepthCompareFunction::Less, .depthWrite = true})
		.BindTextureToSlot(Nebula::Renderer2D::GetWhiteTexture())
		.DrawArrayIndexed(m_VAO);

	m_RendererUBO->SetData(2, "u_Transform", glm::value_ptr(glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f))));

	renderPass->SetShaderPipeline(m_TextureShader)
		.SetUniformBlock("RendererUBO", m_RendererUBO, 2)
		.SetCullMode(Nebula::Graphics::CullMode::Front)
		.SetDepthStencilState({.depthCompareFunction = Nebula::Graphics::DepthCompareFunction::Less, .depthWrite = true})
		.BindTextureToSlot(m_Glass)
		.DrawArrayIndexed(m_VAO);

	m_RendererUBO->SetData(3, "u_Transform", glm::value_ptr(glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f)), glm::vec3(0.01f))));

	renderPass->SetShaderPipeline(m_ModelShader)
		.SetUniformBlock("RendererUBO", m_RendererUBO, 3)
		.SetCullMode(Nebula::Graphics::CullMode::Front)
		.SetDepthStencilState({.depthCompareFunction = Nebula::Graphics::DepthCompareFunction::Less, .depthWrite = true});
	for (auto& mesh : m_Backpack->GetMeshes())
	{
		renderPass->DrawArrayIndexed(mesh->GetVertexArray());
	}

	renderPass->EndRenderPass();
	commandBuffer->Commit();
}

void RendererLayer::OnImGuiRender()
{
	if (m_ShowImGuiMenuBar)
	{
		ImGui::BeginMainMenuBar();
		{
			ImGui::BeginMenu("Nebula Engine", false);
			if (ImGui::BeginMenu("Debug"))
			{
				if (ImGui::MenuItem("Show Debug Window", 0, m_ShowImGuiDebugWindow)) m_ShowImGuiDebugWindow = !m_ShowImGuiDebugWindow;
				ImGui::EndMenu();
			}
		}
		ImGui::EndMainMenuBar();
	}

	if (m_ShowImGuiDebugWindow) Nebula::Debug::ShowDebugWindow();
}

void RendererLayer::OnEvent(Nebula::Platform::Event& event)
{
	m_CameraController.OnEvent(event);

	Nebula::Platform::EventDispatcher dispatcher(event);
	dispatcher.Dispatch<Nebula::Platform::KeyPressedEvent>(NB_BIND_EVENT_FN(OnKeyPressedEvent));
}

bool RendererLayer::OnKeyPressedEvent(Nebula::Platform::KeyPressedEvent& event)
{
	if (event.GetKeyCode() == KeyboardKey::LeftAlt) m_ShowImGuiMenuBar = !m_ShowImGuiMenuBar;
	return false;
}