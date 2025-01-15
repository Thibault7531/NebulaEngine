#pragma once

#include "Timestep.h"
#include "../Events/Event.h"

namespace Nebula
{
	/**
	 * Layer of the application.
	 */
	class Layer
	{
	public:
		Layer(const std::string& name = "Layer");
		virtual ~Layer();

		/**
		 * Code to run when the layer gets added to the application.
		 */
		virtual void OnAttach() {}
		/**
		 * Code to run when the layer gets removed from the application.
		 */
		virtual void OnDetach() {}
		/**
		 * Code to run at every frame.
		 */
		virtual void OnUpdate(Timestep ts) {}
		/**
		 * Code to run if using ImGui Rendering.
		 */
		virtual void OnImGuiRender() {}
		/**
		 * Code to run when an event is received.
		 * @param event Event to be handled by the layer.
		 */
		virtual void OnEvent(Event& event) {}

		/**
		 * Gets the name of the layer.
		 * @return Name of the layer
		 */
		inline const std::string& GetName() const { return m_DebugName; }
	protected:
		std::string m_DebugName;
	};

}
