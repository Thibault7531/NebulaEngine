#include "RendererLayer.h"

RendererLayer::RendererLayer()
    : m_CameraController{16.f/9.f, glm::radians(60.f)}
{
}

void RendererLayer::OnAttach()
{
    m_VAO = Nebula::VertexArray::Create();

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

	Nebula::Ref<Nebula::VertexBuffer> vbo = Nebula::VertexBuffer::Create(vertices, sizeof(vertices));

	Nebula::BufferLayout layout = {
		{ Nebula::ShaderDataType::Float3, "a_Position" },
		{ Nebula::ShaderDataType::Float2, "a_TextCoord"}
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

	Nebula::Ref<Nebula::IndexBuffer> ibo = Nebula::IndexBuffer::Create(indices, std::size(indices));

	m_VAO->SetIndexBuffer(ibo);

	m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/shaders/Texture.slang");
	m_ShaderLibrary.Load(Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/shaders/Model.slang");

	m_Texture = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/textures/crafting_table_front.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});
	m_Glass = Nebula::Texture2D::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/textures/glass.png").string(), {Nebula::Texture::InternalFormat::RGBA8, Nebula::Texture::FilterType::Nearest});

	m_Backpack = Nebula::Model::Create((Nebula::Platform::Files::GetResourcesPath() / "assets/3DRendererExample/models/backpack.fbx").string());

	m_CameraController.GetCamera().setPosition({0.f, 0.f, 3.f});
}

void RendererLayer::OnDetach()
{
}

void RendererLayer::OnUpdate(Nebula::Timestep ts)
{
	m_CameraController.OnUpdate(ts);

	Nebula::RenderCommand::SetClearColor({0.18f, 0.49f, 0.6f, 1.0f});
	Nebula::RenderCommand::Clear();

	Nebula::Renderer::BeginScene(m_CameraController.GetCamera());

	m_Texture->Bind(0);
	m_Glass->Bind(1);

	Nebula::Ref<Nebula::Shader> textureShader = m_ShaderLibrary.Get("Texture");

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 0);
	Nebula::Renderer::Submit(textureShader, m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

	textureShader->Bind();
	textureShader->SetInt("u_Texture", 1);
	Nebula::Renderer::Submit(textureShader, m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f)));

	Nebula::Renderer::SubmitModel(m_ShaderLibrary.Get("Model"), m_Backpack, glm::scale(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 5.0f, 0.0f)), glm::vec3(0.01f)));

	Nebula::Renderer::EndScene();
}

void RendererLayer::OnImGuiRender()
{
}

void RendererLayer::OnEvent(Nebula::Platform::Event& event)
{
	m_CameraController.OnEvent(event);
}
