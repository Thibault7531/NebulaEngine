#pragma once

#include "Layer.h"

namespace Nebula
{
	/**
	 * Layer stack of the application.
	 */
	class LayerStack
	{
	public:
		LayerStack();
		~LayerStack();

		/**
		 * Adds a layer to the layer stack.
		 * @param layer Layer to be added to the layer stack
		 */
		void PushLayer(Layer* layer);
		/**
		 * Adds a layer on top of the layer stack.
		 * @param overlay Layer to be added on top of the layer stack
		 */
		void PushOverlay(Layer* overlay);
		/**
		 * Removes the specified layer.
		 * @param layer Layer to be removed.
		 */
		void PopLayer(const Layer* layer);
		/**
		 * Removes the specified layer.
		 * @param overlay Layer to be removed
		 */
		void PopOverlay(const Layer* overlay);

		std::vector<Layer*>::iterator begin() { return m_Layers.begin(); }
		std::vector<Layer*>::iterator end() { return m_Layers.end(); }
	private:
		std::vector<Layer*> m_Layers;
		unsigned int m_LayerInsertIndex = 0;
	};

}
