#pragma once

#include "Nebula/Layer.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"
#include "backends/imgui_impl_opengl3.h"

namespace Nebula
{

	class NEBULA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		bool OnMouseButtonPressedEvent(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleasedEvent(MouseButtonReleasedEvent& event);
		bool OnMouseMovedEvent(MouseMovedEvent& event);
		bool OnMouseScrolledEvent(MouseScrolledEvent& event);
		bool OnKeyPressedEvent(KeyPressedEvent& event);
		bool OnKeyReleasedEvent(KeyReleasedEvent& event);
		bool OnKeyTypedEvent(KeyTypedEvent& event);
		bool OnWindowResizeEvent(WindowResizeEvent& event);
		static const char* GetClipboard(ImGuiContext*);
		static void SetClipboard(ImGuiContext*, const char* text);

	private:
		ImGuiKey getImGuiKey(int keycode);
	};

}
