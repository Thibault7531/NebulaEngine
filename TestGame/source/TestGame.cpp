#include <Nebula.h>
#include "glad/glad.h"

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
		ImGui::Text((const char*)glGetString(GL_VENDOR));
		ImGui::BulletText("GL_RENDERER: ");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_RENDERER));
		ImGui::BulletText("GL_VERSION: ");
		ImGui::SameLine();
		ImGui::Text((const char*)glGetString(GL_VERSION));

		ImGui::SeparatorText("Rendering Info:");
		ImGui::BulletText("Screen Resolution: " );
		ImGui::SameLine();
		ImGui::Text(std::to_string(Nebula::Application::Get().GetWindow().GetWidth()).c_str());
		ImGui::SameLine();
		ImGui::Text("x");
		ImGui::SameLine();
		ImGui::Text(std::to_string(Nebula::Application::Get().GetWindow().GetHeight()).c_str());
		ImGui::BulletText("Frame Rate: ");
		ImGui::SameLine();
		ImGui::Value("", (float)(1.0f/(ImGui::GetTime() - m_PreviousTime)));
		ImGui::BulletText("Frame Time: ");
		ImGui::SameLine();
		ImGui::Value("", (float)(ImGui::GetTime() - m_PreviousTime));
		m_PreviousTime = ImGui::GetTime();
		ImGui::End();
	}

private:
	double m_PreviousTime;
};

class TestGame : public Nebula::Application
{
public:
	TestGame()
	{
		PushOverlay(new OpenGLDebugWindow());
	}

	~TestGame()
	{
	}

};

Nebula::Application* Nebula::CreateApplication()
{
	return new TestGame;
}
