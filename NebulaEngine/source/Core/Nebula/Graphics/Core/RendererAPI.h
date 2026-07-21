#pragma once

#include "Platform/Nebula/CoreSystems/Graphics/GraphicsAPI.h"

namespace Nebula::Graphics {

	class RendererAPI
	{
	public:
		static void Init(Platform::GraphicsAPI api);
		inline static Platform::GraphicsAPI GetAPI() { return s_API; }

	private:
		static Platform::GraphicsAPI s_API;
	};

}
