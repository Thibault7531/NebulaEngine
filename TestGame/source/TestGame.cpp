#include <Nebula.h>
#include <SDL_keycode.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Nebula/Renderer/Camera.h"
#include "Platform/OpenGL/OpenGLShader.h"

class OpenGLDebugWindow : public Nebula::Layer
{
public:
	OpenGLDebugWindow()
		: m_PreviousTime(ImGui::GetTime()) {}

	void OnImGuiRender() override
	{
		ImGui::Begin("OpenGL Debug Window");

		ImGui::SeparatorText("OpenGL Info:");
		ImGui::BulletText("GL_VENDOR: ");
		ImGui::SameLine();
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		ImGui::BulletText("GL_RENDERER: ");
		ImGui::SameLine();
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		ImGui::BulletText("GL_VERSION: ");
		ImGui::SameLine();
		ImGui::Text(reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		ImGui::SeparatorText("Rendering Info:");
		ImGui::BulletText("Screen Resolution: " );
		ImGui::SameLine();
		ImGui::Text(std::to_string(Nebula::Application::Get().GetWindow().GetWidth()).c_str());
		ImGui::SameLine();
		ImGui::Text("x");
		ImGui::SameLine();
		ImGui::Text(std::to_string(Nebula::Application::Get().GetWindow().GetHeight()).c_str());
		ImGui::BulletText("Frame Rate");
		ImGui::SameLine();
		ImGui::Value("", static_cast<float>(1.0f / (ImGui::GetTime() - m_PreviousTime)));
		ImGui::SameLine();
		ImGui::Text("fps");
		ImGui::BulletText("Frame Time");
		ImGui::SameLine();
		ImGui::Value("", static_cast<float>(ImGui::GetTime() - m_PreviousTime) * 1000.0f);
		ImGui::SameLine();
		ImGui::Text("ms");
		m_PreviousTime = ImGui::GetTime();

		ImGui::End();
	}

private:
	double m_PreviousTime;
};


class TestCube : public Nebula::Layer
{
public:
	TestCube()
		: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_Position(glm::vec3(0.6f, 0.6f, 0.0f)), m_Color(0.0f, 0.0f, 0.0f) {}

	void OnAttach() override
	{
		m_VAO = Nebula::VertexArray::Create();

		float vertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f, 0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f, 0.5f, 0.0f, 0.0f, 1.0f
		};

		Nebula::Ref<Nebula::VertexBuffer> vbo = Nebula::VertexBuffer::Create(vertices, sizeof(vertices));

		Nebula::BufferLayout layout = {
			{ Nebula::ShaderDataType::Float3, "a_Position" },
			{ Nebula::ShaderDataType::Float2, "a_TextCoord"}
		};

		vbo->SetLayout(layout);

		m_VAO->AddVertexBuffer(vbo);

		uint32_t indices[2 * 3] = {
			0, 1, 2,
			2, 3, 0
		};

		Nebula::Ref<Nebula::IndexBuffer> ibo = Nebula::IndexBuffer::Create(indices, std::size(indices));

		m_VAO->SetIndexBuffer(ibo);

		m_Shader = Nebula::Shader::Create("SolidColor", "assets/shaders/SolidColorVS.glsl", "assets/shaders/SolidColorFS.glsl", Nebula::Shader::SOURCE_TYPE::NB_SHADER_FILE);
		m_ShaderLibrary.Add("SolidColor", m_Shader);

		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Nebula::Texture2D::Create("assets/textures/crafting_table_front.png");
		m_Glass = Nebula::Texture2D::Create("assets/textures/glass.png");
	}

	void OnUpdate(Nebula::Timestep ts) override
	{
		if (Nebula::Input::IsKeyPressed(NB_KEY_Z)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(0.0f, 1.0f * ts, 0.0f));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_S)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(0.0f, -1.0f * ts, 0.0f));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_D)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(1.0f * ts, 0.0f, 0.0f));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_Q)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(-1.0f * ts, 0.0f, 0.0f));
		}

		if (Nebula::Input::IsKeyPressed(NB_KEY_A)) {
			float rotation = m_Camera.getRotation();
			m_Camera.setRotation(rotation - 50.0f * ts);
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_E)) {
			float rotation = m_Camera.getRotation();
			m_Camera.setRotation(rotation + 50.0f * ts);
		}

		if (Nebula::Input::IsKeyPressed(NB_KEY_UP)) {
			m_Position += glm::vec3(0.0f, 1.0f, 0.0f) * static_cast<float>(ts);
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_DOWN)) {
			m_Position -= glm::vec3(0.0f, 1.0f, 0.0f) * static_cast<float>(ts);
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_RIGHT)) {
			m_Position += glm::vec3(1.0f, 0.0f, 0.0f) * static_cast<float>(ts);
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_LEFT)) {
			m_Position -= glm::vec3(1.0f, 0.0f, 0.0f) * static_cast<float>(ts);
		}

		Nebula::RenderCommand::SetClearColor({0.18f, 0.49f, 0.6f, 1.0f});
		Nebula::RenderCommand::Clear();

		Nebula::Renderer::BeginScene(m_Camera);

		const glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		m_ShaderLibrary.Get("SolidColor")->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", m_Color);
		m_ShaderLibrary.Get("SolidColor")->Unbind();

		for (int i = 0; i < 4; i++)
		{
			glm::vec3 position = m_Position + glm::vec3(0.11f, 0.0f, 0.0f) * static_cast<float>(i);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

			Nebula::Renderer::Submit(m_ShaderLibrary.Get("SolidColor"), m_VAO, transform);
		}

		m_Texture->Bind(0);
		m_Glass->Bind(1);

		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->UploadUniformInt("u_Texture", 0);
		Nebula::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_VAO, glm::mat4(1.0f));

		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_ShaderLibrary.Get("Texture"))->UploadUniformInt("u_Texture", 1);
		Nebula::Renderer::Submit(m_ShaderLibrary.Get("Texture"), m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(-0.25f, -0.25f, 0.0f)));

		Nebula::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::ColorEdit3("Square color", glm::value_ptr(m_Color));

		ImGui::End();
	}

private:
	Nebula::ShaderLibrary m_ShaderLibrary;

	Nebula::Ref<Nebula::VertexArray> m_VAO;
	Nebula::Ref<Nebula::Shader> m_Shader, m_TextureShader;

	Nebula::Ref<Nebula::Texture2D> m_Texture, m_Glass;

	Nebula::OrthographicCamera m_Camera;

	glm::vec3 m_Position;
	glm::vec3 m_Color;
};


class Cube3D : public Nebula::Layer
{
public:
	Cube3D() : m_Camera(60.0f, 16.0f/9.0f) {}

	virtual void OnAttach() override
	{
		m_VAO = Nebula::VertexArray::Create();

		float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
		};

		Nebula::Ref<Nebula::VertexBuffer> vbo = Nebula::VertexBuffer::Create(vertices, sizeof(vertices));

		Nebula::BufferLayout layout = {
			{ Nebula::ShaderDataType::Float3, "a_Position" },
			{ Nebula::ShaderDataType::Float2, "a_TextCoord"}
		};

		vbo->SetLayout(layout);

		m_VAO->AddVertexBuffer(vbo);

		uint32_t indices[36] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35};

		Nebula::Ref<Nebula::IndexBuffer> ibo = Nebula::IndexBuffer::Create(indices, std::size(indices));

		m_VAO->SetIndexBuffer(ibo);

		m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Nebula::Texture2D::Create("assets/textures/crafting_table_front.png");
		m_Glass = Nebula::Texture2D::Create("assets/textures/glass.png");
	}
	virtual void OnUpdate(Nebula::Timestep ts) override
	{
		if (Nebula::Input::IsKeyPressed(NB_KEY_Z)) {
			glm::vec3 position = m_Camera.getPosition();
			glm::vec3 rotation = m_Camera.getRotation();
			m_Camera.setPosition(position + glm::vec3(2.0f * glm::cos(glm::radians(rotation.y)) * ts, 0.0f, 2.0f * glm::sin(glm::radians(rotation.y)) * ts));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_S)) {
			glm::vec3 position = m_Camera.getPosition();
			glm::vec3 rotation = m_Camera.getRotation();
			m_Camera.setPosition(position + glm::vec3(-2.0f * glm::cos(glm::radians(rotation.y)) * ts, 0.0f, -2.0f * glm::sin(glm::radians(rotation.y)) * ts));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_D)) {
			glm::vec3 position = m_Camera.getPosition();
			glm::vec3 rotation = m_Camera.getRotation();
			m_Camera.setPosition(position + glm::vec3(-2.0f * glm::sin(glm::radians(rotation.y)) * ts, 0.0f, 2.0f * glm::cos(glm::radians(rotation.y)) * ts));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_Q)) {
			glm::vec3 position = m_Camera.getPosition();
			glm::vec3 rotation = m_Camera.getRotation();
			m_Camera.setPosition(position + glm::vec3(2.0f * glm::sin(glm::radians(rotation.y)) * ts, 0.0f, -2.0f * glm::cos(glm::radians(rotation.y)) * ts));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_SPACE)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(0.0f, 2.0f * ts, 0.0f));
		}
		if (Nebula::Input::IsKeyPressed(NB_KEY_LSHIFT)) {
			glm::vec3 position = m_Camera.getPosition();
			m_Camera.setPosition(position + glm::vec3(0.0f, -2.0f * ts, 0.0f));
		}

		Nebula::RenderCommand::SetClearColor({0.18f, 0.49f, 0.6f, 1.0f});
		Nebula::RenderCommand::Clear();

		Nebula::Renderer::BeginScene(m_Camera);

		m_Texture->Bind(0);
		m_Glass->Bind(1);

		Nebula::Ref<Nebula::Shader> textureShader = m_ShaderLibrary.Get("Texture");

		textureShader->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
		Nebula::Renderer::Submit(textureShader, m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -1.0f)));

		textureShader->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 1);
		Nebula::Renderer::Submit(textureShader, m_VAO, glm::translate(glm::mat4(1.0f), glm::vec3(-1.0f, 0.0f, -1.0f)));

		Nebula::Renderer::EndScene();
	}

	virtual void OnEvent(Nebula::Event& event) override
	{
		switch (event.GetEventType())
		{
		case Nebula::EventType::MouseMoved:
			{
				Nebula::MouseMovedEvent mouseEvent = dynamic_cast<Nebula::MouseMovedEvent&>(event);
				if (Nebula::Input::IsMouseButtonPressed(NB_BUTTON_LEFT))
				{
					m_Camera.setRotation(m_Camera.getRotation() + glm::vec3(m_PreviousMousePos.y - mouseEvent.GetY(), mouseEvent.GetX() - m_PreviousMousePos.x, 0.0f) * 0.1f);
				}
			}
		default:;
		}

		m_PreviousMousePos = {Nebula::Input::GetMouseX(), Nebula::Input::GetMouseY()};
	}

private:
	Nebula::Ref<Nebula::VertexArray> m_VAO;

	Nebula::ShaderLibrary m_ShaderLibrary;
	Nebula::Ref<Nebula::Texture2D> m_Texture;
	Nebula::Ref<Nebula::Texture2D> m_Glass;

	Nebula::PerspectiveCamera m_Camera;

	glm::vec2 m_PreviousMousePos;
};


class TestGame : public Nebula::Application
{
public:
	TestGame()
	{
		PushOverlay(new OpenGLDebugWindow());
		//PushLayer(new TestCube());
		PushLayer(new Cube3D());
		this->GetWindow().SetVSync(true);
	}

	~TestGame() override
	= default;

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
