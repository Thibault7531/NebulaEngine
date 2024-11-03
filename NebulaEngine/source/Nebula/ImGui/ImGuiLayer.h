#pragma once

#include "SDL_keycode.h"

#include "Nebula/Layer.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"

namespace Nebula
{

	class NEBULA_API ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer();

		void OnAttach() override;
		void OnDetach() override;
		void OnUpdate() override;
		void OnEvent(Event& event) override;
	private:
		ImGuiKey getImGuiKey(SDL_Keycode keycode);

		Sint64 m_Time;
	};

}
