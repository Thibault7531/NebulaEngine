#include "ImGuiLayer.h"

#include <SDL_clipboard.h>
#include <SDL_events.h>
#include <SDL_video.h>
#include <backends/imgui_impl_opengl3.h>
#include <backends/imgui_impl_opengl3_loader.h>
#include <backends/imgui_impl_sdl3.h>

#include "Nebula/Application.h"
#include "Nebula/Input.h"
#include "Nebula/KeyCodes.h"
#include "Nebula/MouseButtonCodes.h"

struct ImGui_ImplSDL3_Data;
struct ImGui_ImplSDL3_ViewportData;

namespace Nebula
{

	ImGuiLayer::ImGuiLayer()
		: Layer("ImGuiLayer")
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
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
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		SDL_Window* window = static_cast<SDL_Window*>(app.GetWindow().GetNativeWindow());
		SDL_GLContext gl_context = SDL_GL_GetCurrentContext();

		ImGui_ImplSDL3_InitForOpenGL(window, gl_context);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);
		dispatcher.Dispatch<MouseMovedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		dispatcher.Dispatch<MouseScrolledEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
		dispatcher.Dispatch<MouseButtonPressedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		dispatcher.Dispatch<KeyPressedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		dispatcher.Dispatch<KeyReleasedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		dispatcher.Dispatch<KeyTypedEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
		dispatcher.Dispatch<WindowResizeEvent>(NB_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplSDL3_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::OnImGuiRender()
	{
	}

	void ImGuiLayer::Begin()
	{
		ImGuiIO& io = ImGui::GetIO();

		SDL_Event event;
		while (SDL_PollEvent(&event))
			ImGui_ImplSDL3_ProcessEvent(&event);

		SDL_Window* window = SDL_GL_GetCurrentWindow();
		if (io.WantTextInput) {
			SDL_StartTextInput(window);
		} else {
			SDL_StopTextInput(window);
		}

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplSDL3_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End()
	{
		ImGuiIO& io = ImGui::GetIO();

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			SDL_Window* backup_current_window = SDL_GL_GetCurrentWindow();
			SDL_GLContext backup_current_context = SDL_GL_GetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			SDL_GL_MakeCurrent(backup_current_window, backup_current_context);
		}
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		int mouse_button = -1;
		if (event.GetMouseButton() == NB_BUTTON_LEFT) { mouse_button = 0; }
		if (event.GetMouseButton() == NB_BUTTON_RIGHT) { mouse_button = 1; }
		if (event.GetMouseButton() == NB_BUTTON_MIDDLE) { mouse_button = 2; }
		if (event.GetMouseButton() == NB_BUTTON_X1) { mouse_button = 3; }
		if (event.GetMouseButton() == NB_BUTTON_X2) { mouse_button = 4; }
		if (mouse_button == -1)
			return false;

		io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
		io.AddMouseButtonEvent(mouse_button, true);

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		int mouse_button = -1;
		if (event.GetMouseButton() == NB_BUTTON_LEFT) { mouse_button = 0; }
		if (event.GetMouseButton() == NB_BUTTON_RIGHT) { mouse_button = 1; }
		if (event.GetMouseButton() == NB_BUTTON_MIDDLE) { mouse_button = 2; }
		if (event.GetMouseButton() == NB_BUTTON_X1) { mouse_button = 3; }
		if (event.GetMouseButton() == NB_BUTTON_X2) { mouse_button = 4; }
		if (mouse_button == -1)
			return false;

		io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
		io.AddMouseButtonEvent(mouse_button, false);

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(const MouseMovedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.AddMouseSourceEvent(ImGuiMouseSource_Mouse);
		io.AddMousePosEvent(event.GetX(), event.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(const MouseScrolledEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		io.AddMouseWheelEvent(event.GetXOffset(), event.GetYOffset());

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(const KeyPressedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiKey key = getImGuiKey(event.GetKeyCode());
		io.AddKeyEvent(key, true);

		io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
		io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
		io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
		io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(const KeyReleasedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		ImGuiKey key = getImGuiKey(event.GetKeyCode());
		io.AddKeyEvent(key, false);

		io.AddKeyEvent(ImGuiMod_Ctrl, Input::IsKeyPressed(NB_KEY_LCTRL) || Input::IsKeyPressed(NB_KEY_RCTRL));
		io.AddKeyEvent(ImGuiMod_Shift, Input::IsKeyPressed(NB_KEY_LSHIFT) || Input::IsKeyPressed(NB_KEY_RSHIFT));
		io.AddKeyEvent(ImGuiMod_Alt, Input::IsKeyPressed(NB_KEY_LALT) || Input::IsKeyPressed(NB_KEY_RALT));
		io.AddKeyEvent(ImGuiMod_Super, Input::IsKeyPressed(NB_KEY_LGUI) || Input::IsKeyPressed(NB_KEY_RGUI));

		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(const KeyTypedEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();

		int keycode = event.GetKeyCode();
		if (keycode > 0 && keycode < 0x10000)
			io.AddInputCharacter(keycode);

		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(const WindowResizeEvent& event)
	{
		ImGuiIO& io = ImGui::GetIO();
		io.DisplaySize = ImVec2(event.GetWidth(), event.GetHeight());
		io.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);
		glViewport(0, 0, event.GetWidth(), event.GetHeight());

		return false;
	}

	const char* ImGuiLayer::GetClipboard(ImGuiContext*)
	{
		return SDL_GetClipboardText();
	}

	void ImGuiLayer::SetClipboard(ImGuiContext*, const char* text)
	{
		SDL_SetClipboardText(text);
	}

	ImGuiKey ImGuiLayer::getImGuiKey(int keycode)
	{
        switch (keycode)
        {
        case NB_KEY_TAB: return ImGuiKey_Tab;
        case NB_KEY_LEFT: return ImGuiKey_LeftArrow;
        case NB_KEY_RIGHT: return ImGuiKey_RightArrow;
        case NB_KEY_UP: return ImGuiKey_UpArrow;
        case NB_KEY_DOWN: return ImGuiKey_DownArrow;
        case NB_KEY_PAGEUP: return ImGuiKey_PageUp;
        case NB_KEY_PAGEDOWN: return ImGuiKey_PageDown;
        case NB_KEY_HOME: return ImGuiKey_Home;
        case NB_KEY_END: return ImGuiKey_End;
        case NB_KEY_INSERT: return ImGuiKey_Insert;
        case NB_KEY_DELETE: return ImGuiKey_Delete;
        case NB_KEY_BACKSPACE: return ImGuiKey_Backspace;
        case NB_KEY_SPACE: return ImGuiKey_Space;
        case NB_KEY_RETURN: return ImGuiKey_Enter;
        case NB_KEY_ESCAPE: return ImGuiKey_Escape;
        case NB_KEY_APOSTROPHE: return ImGuiKey_Apostrophe;
        case NB_KEY_COMMA: return ImGuiKey_Comma;
        case NB_KEY_MINUS: return ImGuiKey_Minus;
        case NB_KEY_PERIOD: return ImGuiKey_Period;
        case NB_KEY_SLASH: return ImGuiKey_Slash;
        case NB_KEY_SEMICOLON: return ImGuiKey_Semicolon;
        case NB_KEY_EQUALS: return ImGuiKey_Equal;
        case NB_KEY_LEFTBRACKET: return ImGuiKey_LeftBracket;
        case NB_KEY_BACKSLASH: return ImGuiKey_Backslash;
        case NB_KEY_RIGHTBRACKET: return ImGuiKey_RightBracket;
        case NB_KEY_GRAVE: return ImGuiKey_GraveAccent;
        case NB_KEY_CAPSLOCK: return ImGuiKey_CapsLock;
        case NB_KEY_SCROLLLOCK: return ImGuiKey_ScrollLock;
        case NB_KEY_NUMLOCKCLEAR: return ImGuiKey_NumLock;
        case NB_KEY_PRINTSCREEN: return ImGuiKey_PrintScreen;
        case NB_KEY_PAUSE: return ImGuiKey_Pause;
        case NB_KEY_LCTRL: return ImGuiKey_LeftCtrl;
        case NB_KEY_LSHIFT: return ImGuiKey_LeftShift;
        case NB_KEY_LALT: return ImGuiKey_LeftAlt;
        case NB_KEY_LGUI: return ImGuiKey_LeftSuper;
        case NB_KEY_RCTRL: return ImGuiKey_RightCtrl;
        case NB_KEY_RSHIFT: return ImGuiKey_RightShift;
        case NB_KEY_RALT: return ImGuiKey_RightAlt;
        case NB_KEY_RGUI: return ImGuiKey_RightSuper;
        case NB_KEY_APPLICATION: return ImGuiKey_Menu;
        case NB_KEY_0: return ImGuiKey_0;
        case NB_KEY_1: return ImGuiKey_1;
        case NB_KEY_2: return ImGuiKey_2;
        case NB_KEY_3: return ImGuiKey_3;
        case NB_KEY_4: return ImGuiKey_4;
        case NB_KEY_5: return ImGuiKey_5;
        case NB_KEY_6: return ImGuiKey_6;
        case NB_KEY_7: return ImGuiKey_7;
        case NB_KEY_8: return ImGuiKey_8;
        case NB_KEY_9: return ImGuiKey_9;
        case NB_KEY_A: return ImGuiKey_A;
        case NB_KEY_B: return ImGuiKey_B;
        case NB_KEY_C: return ImGuiKey_C;
        case NB_KEY_D: return ImGuiKey_D;
        case NB_KEY_E: return ImGuiKey_E;
        case NB_KEY_F: return ImGuiKey_F;
        case NB_KEY_G: return ImGuiKey_G;
        case NB_KEY_H: return ImGuiKey_H;
        case NB_KEY_I: return ImGuiKey_I;
        case NB_KEY_J: return ImGuiKey_J;
        case NB_KEY_K: return ImGuiKey_K;
        case NB_KEY_L: return ImGuiKey_L;
        case NB_KEY_M: return ImGuiKey_M;
        case NB_KEY_N: return ImGuiKey_N;
        case NB_KEY_O: return ImGuiKey_O;
        case NB_KEY_P: return ImGuiKey_P;
        case NB_KEY_Q: return ImGuiKey_Q;
        case NB_KEY_R: return ImGuiKey_R;
        case NB_KEY_S: return ImGuiKey_S;
        case NB_KEY_T: return ImGuiKey_T;
        case NB_KEY_U: return ImGuiKey_U;
        case NB_KEY_V: return ImGuiKey_V;
        case NB_KEY_W: return ImGuiKey_W;
        case NB_KEY_X: return ImGuiKey_X;
        case NB_KEY_Y: return ImGuiKey_Y;
        case NB_KEY_Z: return ImGuiKey_Z;
        case NB_KEY_F1: return ImGuiKey_F1;
        case NB_KEY_F2: return ImGuiKey_F2;
        case NB_KEY_F3: return ImGuiKey_F3;
        case NB_KEY_F4: return ImGuiKey_F4;
        case NB_KEY_F5: return ImGuiKey_F5;
        case NB_KEY_F6: return ImGuiKey_F6;
        case NB_KEY_F7: return ImGuiKey_F7;
        case NB_KEY_F8: return ImGuiKey_F8;
        case NB_KEY_F9: return ImGuiKey_F9;
        case NB_KEY_F10: return ImGuiKey_F10;
        case NB_KEY_F11: return ImGuiKey_F11;
        case NB_KEY_F12: return ImGuiKey_F12;
        case NB_KEY_F13: return ImGuiKey_F13;
        case NB_KEY_F14: return ImGuiKey_F14;
        case NB_KEY_F15: return ImGuiKey_F15;
        case NB_KEY_F16: return ImGuiKey_F16;
        case NB_KEY_F17: return ImGuiKey_F17;
        case NB_KEY_F18: return ImGuiKey_F18;
        case NB_KEY_F19: return ImGuiKey_F19;
        case NB_KEY_F20: return ImGuiKey_F20;
        case NB_KEY_F21: return ImGuiKey_F21;
        case NB_KEY_F22: return ImGuiKey_F22;
        case NB_KEY_F23: return ImGuiKey_F23;
        case NB_KEY_F24: return ImGuiKey_F24;
        case NB_KEY_AC_BACK: return ImGuiKey_AppBack;
        case NB_KEY_AC_FORWARD: return ImGuiKey_AppForward;
        default: break;
        }
        return ImGuiKey_None;
	}
}
