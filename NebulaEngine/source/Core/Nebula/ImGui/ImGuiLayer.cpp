#include "ImGuiLayer.h"

#include <ImGuizmo.h>

#include "ImGuiRenderer.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/Graphics/Core/RendererAPI.h"
#include "Platform/Nebula/CoreSystems/Input/Input.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula
{
	ImGuiFonts ImGuiLayer::s_Fonts;
	Platform::Theme::ThemeType ImGuiLayer::s_ImGuiTheme = Platform::Theme::ThemeType::Dark;
	bool ImGuiLayer::s_AutoTheme = true;

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer") {}

	ImGuiLayer::~ImGuiLayer()
	= default;

	void ImGuiLayer::OnAttach()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Initializing ImGui");

		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;

		io.ConfigWindowsMoveFromTitleBarOnly = true;

		char* iniLocation = static_cast<char*>(malloc(strlen(Platform::Files::GetUserDataPath().string().c_str()) + strlen("imgui.ini") + 1));
		strcpy(iniLocation, (Platform::Files::GetUserDataPath() / "imgui.ini").string().c_str());
		io.IniFilename = iniLocation;

		ImGui::StyleColorsDark();
		if (Platform::Theme::GetTheme() == Platform::Theme::ThemeType::Dark) SetDarkTheme();
		else SetLightTheme();

		m_ImGuiPlatform = CreateRef<ImGuiPlatform>();
		m_ImGuiPlatform->Init(Platform::WindowManager::Get().GetMainWindow());

		ImGuiRenderer::Init();

		// Font configuration
		ImFontConfig config;
		config.OversampleH = 3;
		config.OversampleV = 3;
		config.PixelSnapH = true;

		// Default font sizes
		float baseSize = 16.0f;
		float smallSize = 14.0f;
		float largeSize = 18.0f;

		// Load fonts
		ImFontConfig iconConfig;
		iconConfig.MergeMode = true;
		iconConfig.PixelSnapH = true;
		static const ImWchar iconRanges[] = { 0xf000, 0xf3ff, 0 };

		io.FontDefault = io.Fonts->AddFontFromFileTTF(std::format("{}/assets/NebulaEngine/fonts/Inter_18pt-Regular.ttf", Platform::Files::GetResourcesPath().string()).c_str(), baseSize, &config);
		ImFont* fontBold = io.Fonts->AddFontFromFileTTF(std::format("{}/assets/NebulaEngine/fonts/Inter_18pt-SemiBold.ttf", Platform::Files::GetResourcesPath().string()).c_str(), baseSize, &config);
		ImFont* fontLarge = io.Fonts->AddFontFromFileTTF(std::format("{}/assets/NebulaEngine/fonts/Inter_18pt-Medium.ttf", Platform::Files::GetResourcesPath().string()).c_str(), largeSize, &config);
		ImFont* fontSmall = io.Fonts->AddFontFromFileTTF(std::format("{}/assets/NebulaEngine/fonts/Inter_18pt-Regular.ttf", Platform::Files::GetResourcesPath().string()).c_str(), smallSize, &config);

		io.Fonts->Build();

		s_Fonts.Regular = io.FontDefault;
		s_Fonts.Bold = fontBold;
		s_Fonts.Large = fontLarge;
		s_Fonts.Small = fontSmall;
	}

	void ImGuiLayer::OnEvent(Platform::Event& event)
	{
		m_ImGuiPlatform->OnEvent(event);

		if (m_BlockEvents) {
			ImGuiIO& io = ImGui::GetIO();
			event.Handled |= event.IsInCategory(Platform::EventCategoryMouse) & io.WantCaptureMouse;
			event.Handled |= event.IsInCategory(Platform::EventCategoryKeyboard) & io.WantCaptureKeyboard;
			if (event.GetEventType() == Platform::EventType::KeyPressed) event.Handled &= !Platform::Input::IsGlobalKey(dynamic_cast<Platform::KeyPressedEvent*>(&event)->GetKeyCode());
		}
	}

	void ImGuiLayer::OnDetach()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Shutting down ImGui");

		ImGuiRenderer::Shutdown();
		m_ImGuiPlatform->Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::Begin(Timestep ts) const
	{
		NB_PROFILE_FUNCTION();

		if (s_AutoTheme)
		{
			Platform::Theme::ThemeType theme = Platform::Theme::GetTheme();
			if (theme != s_ImGuiTheme && theme == Platform::Theme::ThemeType::Dark) SetDarkTheme();
			else if (theme != s_ImGuiTheme) SetLightTheme();
		}

		ImGuiRenderer::BeginFrame();
		m_ImGuiPlatform->NewFrame(ts);
		ImGui::NewFrame();
		ImGuizmo::BeginFrame();
	}

	void ImGuiLayer::End() const
	{
		NB_PROFILE_FUNCTION();

		ImGuiIO& io = ImGui::GetIO();

		ImGui::Render();
		ImGuiRenderer::RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			if (Graphics::RendererAPI::GetAPI() == Platform::GraphicsAPI::OpenGL460) Platform::WindowManager::Get().GetMainWindow()->GetSurface()->NewFrame();
		}
	}

	void ImGuiLayer::SetDarkTheme()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		s_ImGuiTheme = Platform::Theme::ThemeType::Dark;

		// Style
		style.WindowPadding = ImVec2(12, 12);
		style.FramePadding = ImVec2(6, 6);
		style.ItemSpacing = ImVec2(8, 6);
		style.ItemInnerSpacing = ImVec2(6, 6);
		style.ScrollbarSize = 16.0f;
		style.GrabMinSize = 8.0f;

		// Rounding
		style.WindowRounding = 8.0f;
		style.ChildRounding = 6.0f;
		style.FrameRounding = 6.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 12.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 6.0f;

		// Alignement
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

		// Border
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 0.0f;

		// Main
		colors[ImGuiCol_WindowBg] = ImVec4(0.119f, 0.123f, 0.129f, 1.0f);
		colors[ImGuiCol_PopupBg] = ImVec4(0.08f, 0.08f, 0.10f, 0.94f);
		colors[ImGuiCol_Border] = ImVec4(0.15f, 0.16f, 0.18f, 1.00f);
		colors[ImGuiCol_FrameBg] = ImVec4(0.15f, 0.16f, 0.18f, 1.0f);
		colors[ImGuiCol_FrameBgHovered] = ImVec4(0.19f, 0.20f, 0.22f, 1.0f);
		colors[ImGuiCol_FrameBgActive] = ImVec4(0.24f, 0.25f, 0.29f, 1.0f);

		// Menu Bars
		colors[ImGuiCol_MenuBarBg] = ImVec4(0.14f, 0.14f, 0.14f, 1.00f);

		// Titles
		colors[ImGuiCol_TitleBg] = ImVec4(0.09f, 0.09f, 0.09f, 1.00f);
		colors[ImGuiCol_TitleBgActive] = ImVec4(0.12f, 0.30f, 0.35f, 1.00f);
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.09f, 0.09f, 0.11f, 0.75f);

		// Accent colors
		ImVec4 accentColor = ImVec4(0.18f, 0.59f, 0.71f, 1.00f);         // Main accent
		ImVec4 accentColorHovered = ImVec4(0.22f, 0.65f, 0.77f, 1.00f);  // Lighter shade
		ImVec4 accentColorActive = ImVec4(0.15f, 0.51f, 0.63f, 1.00f);   // Darker shade
		ImVec4 accentColorTransparent = ImVec4(0.18f, 0.59f, 0.71f, 0.40f); // Transparent version

		// Interactive elements
		colors[ImGuiCol_CheckMark] = accentColor;
		colors[ImGuiCol_SliderGrab] = accentColor;
		colors[ImGuiCol_SliderGrabActive] = accentColorActive;
		colors[ImGuiCol_Button] = accentColorTransparent;
		colors[ImGuiCol_ButtonHovered] = accentColorHovered;
		colors[ImGuiCol_ButtonActive] = accentColorActive;

		// Headers
		colors[ImGuiCol_Header] = ImVec4(0.18f, 0.59f, 0.71f, 0.31f);
		colors[ImGuiCol_HeaderHovered] = ImVec4(0.22f, 0.65f, 0.77f, 0.40f);
		colors[ImGuiCol_HeaderActive] = ImVec4(0.15f, 0.51f, 0.63f, 0.50f);

		// Tabs
		colors[ImGuiCol_Tab] = ImVec4(0.11f, 0.27f, 0.32f, 0.80f);
		colors[ImGuiCol_TabHovered] = accentColorHovered;
		colors[ImGuiCol_TabActive] = ImVec4(0.18f, 0.59f, 0.71f, 0.84f);
		colors[ImGuiCol_TabSelected] = accentColor;
		colors[ImGuiCol_TabSelectedOverline].w = 0.0f;
		colors[ImGuiCol_TabUnfocused] = ImVec4(0.13f, 0.14f, 0.16f, 0.97f);
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.15f, 0.44f, 0.53f, 0.89f);

		// Docking and resizing
		colors[ImGuiCol_DockingPreview] = ImVec4(0.18f, 0.59f, 0.71f, 0.70f);
		colors[ImGuiCol_DockingEmptyBg] = ImVec4(0.08f, 0.08f, 0.08f, 1.00f);
		colors[ImGuiCol_ResizeGrip] = ImVec4(0.18f, 0.59f, 0.71f, 0.20f);
		colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.22f, 0.65f, 0.77f, 0.67f);
		colors[ImGuiCol_ResizeGripActive] = ImVec4(0.15f, 0.51f, 0.63f, 0.95f);

		// Text and selection
		colors[ImGuiCol_Text] = ImVec4(0.95f, 0.95f, 0.95f, 1.00f);
		colors[ImGuiCol_TextDisabled] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
		colors[ImGuiCol_TextSelectedBg] = ImVec4(0.18f, 0.59f, 0.71f, 0.35f);

		// Separators
		colors[ImGuiCol_Separator] = ImVec4(0.2f, 0.22f, 0.25f, 1.0f);
		colors[ImGuiCol_SeparatorHovered] = ImVec4(0.22f, 0.65f, 0.77f, 0.78f);
		colors[ImGuiCol_SeparatorActive] = ImVec4(0.15f, 0.51f, 0.63f, 1.00f);

		// Scrollbar
		colors[ImGuiCol_ScrollbarBg] = ImVec4(0.02f, 0.02f, 0.02f, 0.53f);
		colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.31f, 0.31f, 0.31f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.41f, 0.41f, 0.41f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.51f, 0.51f, 0.51f, 1.00f);
	}

	void ImGuiLayer::SetLightTheme()
	{
		ImGuiStyle& style = ImGui::GetStyle();
		auto& colors = style.Colors;
		s_ImGuiTheme = Platform::Theme::ThemeType::Light;

		// Style
		style.WindowPadding = ImVec2(12, 12);
		style.FramePadding = ImVec2(6, 6);
		style.ItemSpacing = ImVec2(8, 6);
		style.ItemInnerSpacing = ImVec2(6, 6);
		style.ScrollbarSize = 16.0f;
		style.GrabMinSize = 8.0f;

		// Rounding
		style.WindowRounding = 8.0f;
		style.ChildRounding = 6.0f;
		style.FrameRounding = 6.0f;
		style.PopupRounding = 6.0f;
		style.ScrollbarRounding = 12.0f;
		style.GrabRounding = 4.0f;
		style.TabRounding = 6.0f;

		// Alignment
		style.WindowTitleAlign = ImVec2(0.5f, 0.5f);
		style.WindowMenuButtonPosition = ImGuiDir_None;
		style.ColorButtonPosition = ImGuiDir_Right;
		style.ButtonTextAlign = ImVec2(0.5f, 0.5f);
		style.SelectableTextAlign = ImVec2(0.0f, 0.5f);

		// Border
		style.WindowBorderSize = 1.0f;
		style.ChildBorderSize = 1.0f;
		style.PopupBorderSize = 1.0f;
		style.FrameBorderSize = 1.0f;
		style.TabBorderSize = 1.0f;

		// Main Colors (Light Backgrounds)
		colors[ImGuiCol_WindowBg]         = ImVec4(0.94f, 0.94f, 0.95f, 1.00f); // Soft light grey
		colors[ImGuiCol_ChildBg]          = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_PopupBg]          = ImVec4(1.00f, 1.00f, 1.00f, 0.98f); // Pure white for popups to pop
		colors[ImGuiCol_Border]           = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_BorderShadow]     = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
		colors[ImGuiCol_FrameBg]          = ImVec4(0.88f, 0.88f, 0.90f, 1.00f);
		colors[ImGuiCol_FrameBgHovered]   = ImVec4(0.82f, 0.82f, 0.85f, 1.00f);
		colors[ImGuiCol_FrameBgActive]    = ImVec4(0.75f, 0.75f, 0.80f, 1.00f);
		colors[ImGuiCol_MenuBarBg]        = ImVec4(0.88f, 0.88f, 0.89f, 1.00f);

		// Menu Bars
		colors[ImGuiCol_MenuBarBg]        = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);

		// Titles
		colors[ImGuiCol_TitleBg]          = ImVec4(0.88f, 0.88f, 0.90f, 1.00f); // Inactive: Very subtle grey
		colors[ImGuiCol_TitleBgActive]    = ImVec4(0.82f, 0.84f, 0.88f, 1.00f); // Active: Soft blue-grey tint
		colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.88f, 0.88f, 0.90f, 0.75f);

		// Accent colors (Kept consistent with your brand colors but adjusted for light background)
		ImVec4 accentColor            = ImVec4(0.18f, 0.59f, 0.71f, 1.00f); // The "Nebula Blue"
		ImVec4 accentColorHovered     = ImVec4(0.22f, 0.65f, 0.77f, 1.00f);
		ImVec4 accentColorActive      = ImVec4(0.15f, 0.51f, 0.63f, 1.00f);
		ImVec4 accentColorTransparent = ImVec4(0.18f, 0.59f, 0.71f, 0.15f);

		// Interactive elements
		colors[ImGuiCol_CheckMark]        = accentColor;
		colors[ImGuiCol_SliderGrab]       = accentColor;
		colors[ImGuiCol_SliderGrabActive] = accentColorActive;
		colors[ImGuiCol_Button]           = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_ButtonHovered]    = accentColor; // Blue hover for modern feel
		colors[ImGuiCol_ButtonActive]     = accentColorActive;

		// Headers
		colors[ImGuiCol_Header]           = ImVec4(0.18f, 0.59f, 0.71f, 0.20f);
		colors[ImGuiCol_HeaderHovered]    = ImVec4(0.18f, 0.59f, 0.71f, 0.35f);
		colors[ImGuiCol_HeaderActive]     = ImVec4(0.18f, 0.59f, 0.71f, 0.50f);

		// Tabs
		colors[ImGuiCol_Tab]                = ImVec4(0.80f, 0.80f, 0.82f, 1.00f); // Darker base for inactive tabs
		colors[ImGuiCol_TabHovered]         = accentColorHovered;                 // Visual feedback on hover
		colors[ImGuiCol_TabActive]          = ImVec4(0.94f, 0.94f, 0.95f, 1.00f); // Matches WindowBg exactly
		colors[ImGuiCol_TabSelected]        = accentColor;                        // Brand blue highlight
		colors[ImGuiCol_TabSelectedOverline] = accentColor;                       // Stronger visual indicator on top
		colors[ImGuiCol_TabSelectedOverline].w = 0.0f;                            // Make the overline visible
		colors[ImGuiCol_TabUnfocused]       = ImVec4(0.82f, 0.82f, 0.84f, 1.00f); // Subtle background for the bar
		colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.88f, 0.88f, 0.90f, 1.00f); // Slight lift for active but unfocused

		// Docking and resizing
		colors[ImGuiCol_DockingPreview]     = ImVec4(0.18f, 0.59f, 0.71f, 0.40f);
		colors[ImGuiCol_DockingEmptyBg]     = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
		colors[ImGuiCol_ResizeGrip]         = ImVec4(0.32f, 0.48f, 0.55f, 0.10f);
		colors[ImGuiCol_ResizeGripHovered]  = ImVec4(0.32f, 0.48f, 0.55f, 0.50f);
		colors[ImGuiCol_ResizeGripActive]   = ImVec4(0.32f, 0.48f, 0.55f, 0.80f);

		// Text and selection (Dark Text)
		colors[ImGuiCol_Text]               = ImVec4(0.15f, 0.15f, 0.16f, 1.00f); // Deep charcoal, not pure black
		colors[ImGuiCol_TextDisabled]       = ImVec4(0.50f, 0.50f, 0.55f, 1.00f);
		colors[ImGuiCol_TextSelectedBg]     = ImVec4(0.18f, 0.59f, 0.71f, 0.30f);

		// Separators
		colors[ImGuiCol_Separator]          = ImVec4(0.72f, 0.72f, 0.75f, 0.60f);
		colors[ImGuiCol_SeparatorHovered]   = accentColor;
		colors[ImGuiCol_SeparatorActive]    = accentColorActive;

		// Scrollbar
		colors[ImGuiCol_Separator]          = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
		colors[ImGuiCol_ScrollbarBg]        = ImVec4(0.90f, 0.90f, 0.92f, 1.00f);
		colors[ImGuiCol_ScrollbarGrab]      = ImVec4(0.70f, 0.70f, 0.75f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.65f, 0.65f, 0.68f, 1.00f);
		colors[ImGuiCol_ScrollbarGrabActive]  = ImVec4(0.55f, 0.55f, 0.58f, 1.00f);
	}
}
