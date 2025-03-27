#pragma once

#include <backends/imgui_impl_opengl3.h>

#include "../Core/Layer.h"
#include "Nebula/Events/ApplicationEvent.h"
#include "Nebula/Events/KeyEvent.h"
#include "Nebula/Events/MouseEvent.h"

namespace Nebula
{

	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() override;

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

		void Begin();
		void End();

		void BlockEvents(const bool block) { m_BlockEvents = block; }

	private:
		bool OnMouseButtonPressedEvent(const MouseButtonPressedEvent& event);
		bool OnMouseButtonReleasedEvent(const MouseButtonReleasedEvent& event);
		bool OnMouseMovedEvent(const MouseMovedEvent& event);
		bool OnMouseScrolledEvent(const MouseScrolledEvent& event);
		bool OnKeyPressedEvent(const KeyPressedEvent& event);
		bool OnKeyReleasedEvent(const KeyReleasedEvent& event);
		bool OnKeyTypedEvent(const KeyTypedEvent& event);
		bool OnWindowResizeEvent(const WindowResizeEvent& event);
		static const char* GetClipboard(ImGuiContext*);
		static void SetClipboard(ImGuiContext*, const char* text);

	private:
		ImGuiKey getImGuiKey(int keycode);

	private:
		bool m_BlockEvents = true;

	};

}
