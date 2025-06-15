#include "ImGuiLayer.h"

#include <SDL_events.h>
#include <SDL_video.h>

#include "Nebula/Core/Application.h"

namespace Nebula
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
		NB_CORE_INFO("Creating ImGui Layer");
	}

	ImGuiLayer::~ImGuiLayer()
	= default;

	void ImGuiLayer::OnAttach()
	{
		NB_PROFILE_FUNCTION();

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
			style.FrameRounding = 4.0f;

			style.WindowPadding = ImVec2(8, 8);
			style.FramePadding = ImVec2(3, 3);
			style.GrabMinSize = 6.f;

			style.WindowRounding = 8.0f;
			style.ChildRounding = 8.0f;
			style.GrabRounding = 4.0f;
			style.TabRounding = 4.0f;

			style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
			style.WindowMenuButtonPosition = ImGuiDir_None;
			style.ColorButtonPosition = ImGuiDir_Left;
			style.SeparatorTextAlign = ImVec2(0.0f, 0.5f);
			style.SeparatorTextPadding = ImVec2(15.f, 3.f);
			style.DockingSeparatorSize = 1.0f;

			style.WindowBorderSize = 0.0f;
			style.ChildBorderSize = 0.0f;
			style.PopupRounding = 8.0f;
			style.PopupBorderSize = 0.0f;
			style.FrameBorderSize = 0.0f;
		}

		SetDarkTheme();

		m_ImGuiPlatform = ImGuiPlatform::Create();
		m_ImGuiPlatform->Init(&Application::Get().GetWindow());

		m_ImGuiRenderer = ImGuiRenderer::Create();
		m_ImGuiRenderer->Init();

		io.Fonts->AddFontFromFileTTF("assets/NebulaEngine/fonts/JetBrainsMono-Bold.ttf", 17);
		io.FontDefault = io.Fonts->AddFontFromFileTTF("assets/NebulaEngine/fonts/JetBrainsMono-Regular.ttf", 17);
		io.Fonts->Build();
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		m_ImGuiPlatform->OnEvent(event);

		if (m_BlockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(EventCategoryKeyboard) & io.WantCaptureKeyboard;
		}
	}

	void ImGuiLayer::OnDetach()
	{
		NB_PROFILE_FUNCTION();

		m_ImGuiRenderer->Shutdown();
		m_ImGuiPlatform->Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::Begin()
	{
		NB_PROFILE_FUNCTION();

		m_ImGuiPlatform->SetInputStatus();

		m_ImGuiRenderer->NewFrame();
		m_ImGuiPlatform->NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		NB_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::Render();
		m_ImGuiRenderer->Render(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			Application::Get().GetWindow().SetCurrentContext();
		}
	}

	void ImGuiLayer::SetDarkTheme()
	{
		auto& colors = ImGui::GetStyle().Colors;

		colors[ImGuiCol_FrameBg] = ImVec4(0.16f, 0.16f, 0.16f, 0.54f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.26f, 0.9f, 0.98f, 0.40f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.26f, 0.9f, 0.98f, 0.67f);

		colors[ImGuiCol_TitleBg] = ImVec4(0.f, 0.0f, 0.0f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.16f, 0.44f, 0.48f, 1.00f);

		colors[ImGuiCol_CheckMark] = ImVec4(0.26f, 0.9f, 0.98f, 1.00f);
		colors[ImGuiCol_SliderGrab] = ImVec4(0.24f, 0.81f, 0.88f, 1.00f);
		colors[ImGuiCol_SliderGrabActive] = ImVec4(0.26f, 0.9f, 0.98f, 1.00f);
		colors[ImGuiCol_Button] = ImVec4(0.26f, 0.9f, 0.98f, 0.4f);
		colors[ImGuiCol_ButtonHovered] = ImVec4(0.26f, 0.9f, 0.98f, 1.0f);
		colors[ImGuiCol_ButtonActive] = ImVec4(0.06f, 0.88f, 0.98f, 1.0f);

		colors[ImGuiCol_Header] = ImVec4(0.26f, 0.9f, 0.98f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.9f, 0.98f, 0.8f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.26f, 0.9f, 0.98f, 1.0f);

		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.10f, 0.68f, 0.75f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.10f, 0.68f, 0.75f, 1.00f);

		colors[ImGuiCol_ResizeGrip] = ImVec4(0.26f, 0.9f, 0.98f, 0.2f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.26f, 0.9f, 0.98f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.26f, 0.9f, 0.98f, 0.95f);

		colors[ImGuiCol_TabHovered] = ImVec4(0.26f, 0.9f, 0.98f, 0.8f);
		colors[ImGuiCol_Tab] = ImVec4(0.18f, 0.54f, 0.58f, 0.86f);
		colors[ImGuiCol_TabSelected] = ImVec4(0.20f, 0.63f, 0.68f, 1.0f);
		colors[ImGuiCol_TabSelectedOverline] = ImVec4(0.26f, 0.9f, 0.98f, 1.0f);
		colors[ImGuiCol_TabDimmed] = ImVec4(0.07f, 0.14f, 0.15f, 0.97f);
		colors[ImGuiCol_TabDimmedSelected] = ImVec4(0.13f, 0.39f, 0.42f, 1.0f);

		colors[ImGuiCol_DockingPreview] = ImVec4(0.26f, 0.9f, 0.98f, 0.70f);

		colors[ImGuiCol_TextLink] = ImVec4(0.26f, 0.9f, 0.98f, 0.70f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.26f, 0.9f, 0.98f, 0.35f);

		colors[ImGuiCol_NavCursor] = ImVec4(0.26f, 0.9f, 0.98f, 1.0f);
	}


}
