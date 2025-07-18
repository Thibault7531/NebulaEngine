#pragma once

#include "ImGuiPlatform.h"
#include "ImGuiRenderer.h"
#include "Nebula/CoreSystems/Application/Layer.h"

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
		virtual ~ImGuiLayer();

		virtual void OnAttach() override;
		virtual void OnDetach() override;
		virtual void OnEvent(Event& event) override;
		virtual void OnImGuiRender() override;

		void Begin() const;
		void End() const;

		void BlockEvents(const bool block) { m_BlockEvents = block; }

	private:
		static void SetDarkTheme();

	private:
		bool m_BlockEvents = true;

		Ref<ImGuiPlatform> m_ImGuiPlatform;
		Ref<ImGuiRenderer> m_ImGuiRenderer;

	public:
		static ImGuiFonts s_Fonts;
	};

}
