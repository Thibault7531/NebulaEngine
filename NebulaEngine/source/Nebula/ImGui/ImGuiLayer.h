#pragma once

#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "Nebula/Core/Layer.h"

namespace Nebula
{
	struct ImGuiFonts
	{
		ImFont* Regular = nullptr;
		ImFont* Bold = nullptr;
		ImFont* Large = nullptr;
		ImFont* Small = nullptr;
	};

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
		void SetDarkTheme();

	private:
		bool m_BlockEvents = true;

		Ref<ImGuiPlatform> m_ImGuiPlatform;
		Ref<ImGuiRenderer> m_ImGuiRenderer;

	public:
		static ImGuiFonts s_Fonts;
	};

}
