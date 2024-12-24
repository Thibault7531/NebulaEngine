#include <Nebula.h>
#include <SDL_keycode.h>
#include <glad/glad.h>

#include <glm/gtc/matrix_transform.hpp>

#include "glm/gtc/type_ptr.inl"
#include "Nebula/Renderer/OrthographicCamera.h"
#include "Platform/OpenGL/OpenGLShader.h"
#include "spdlog/fmt/bundled/color.h"

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
		: m_Camera(-1.6f, 1.6f, -0.9f, 0.9f), m_Position(glm::vec3(0.0f, 0.0f, 0.0f)), m_Color(0.0f, 0.0f, 0.0f) {}

	void OnAttach() override
	{
		m_VAO.reset(Nebula::VertexArray::Create());

		float vertices[4 * 7] = {
			-0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
			0.5f, 0.5f, 0.0f, 0.0f, 1.0f, 1.0f, 1.0f,
			-0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f
		};

		Nebula::Ref<Nebula::VertexBuffer> vbo;
		vbo.reset(Nebula::VertexBuffer::Create(vertices, sizeof(vertices)));

		Nebula::BufferLayout layout = {
			{ Nebula::ShaderDataType::Float3, "a_Position" },
			{ Nebula::ShaderDataType::Float4, "a_Color"}
		};

		vbo->SetLayout(layout);

		m_VAO->AddVertexBuffer(vbo);

		uint32_t indices[2 * 3] = {
			0, 1, 2,
			2, 1, 3
		};

		Nebula::Ref<Nebula::IndexBuffer> ibo;
		ibo.reset(Nebula::IndexBuffer::Create(indices, std::size(indices)));

		m_VAO->SetIndexBuffer(ibo);

		const std::string vertexSrc = R"(
			#version 330 core

			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);
			}

		)";

		const std::string fragmentSrc = R"(
			#version 330 core

			layout(location = 0) out vec4 color;

			uniform vec3 u_Color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}

		)";

		m_Shader.reset(Nebula::Shader::Create(vertexSrc, fragmentSrc));
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

		m_Shader->Bind();
		std::dynamic_pointer_cast<Nebula::OpenGLShader>(m_Shader)->UploadUniformFloat3("u_Color", m_Color);

		for (int i = 0; i < 4; i++)
		{
			glm::vec3 position = m_Position + glm::vec3(0.11f, 0.0f, 0.0f) * static_cast<float>(i);
			glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) * scale;

			Nebula::Renderer::Submit(m_Shader, m_VAO, transform);
		}

		Nebula::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");

		ImGui::ColorEdit3("Square color", glm::value_ptr(m_Color));

		ImGui::End();
	}

private:
	Nebula::Ref<Nebula::VertexArray> m_VAO;
	Nebula::Ref<Nebula::Shader> m_Shader;

	Nebula::OrthographicCamera m_Camera;

	glm::vec3 m_Position;
	glm::vec3 m_Color;
};


class TestGame : public Nebula::Application
{
public:
	TestGame()
	{
		PushOverlay(new OpenGLDebugWindow());
		PushLayer(new TestCube());
		this->GetWindow().SetVSync(true);
	}

	~TestGame() override
	= default;

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
