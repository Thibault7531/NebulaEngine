#pragma once

#include "Platform/Nebula/CoreSystems/Graphics/GraphicsAPI.h"

namespace Nebula::Graphics {

	class RendererAPI
	{
	public:
		static void Init(Platform::GraphicsAPI api);
		inline static Platform::GraphicsAPI GetAPI() { return s_API; }

		inline static std::string GetAPIString()
		{
			return GraphicsAPIStr(s_API);
		}
		inline static std::string GraphicsAPIStr(Platform::GraphicsAPI api)
		{
			switch (api)
			{
			case Platform::GraphicsAPI::None: return "None";
			case Platform::GraphicsAPI::Metal3: return "Metal 3";
			case Platform::GraphicsAPI::OpenGL460: return "OpenGL 4.6.0";
			case Platform::GraphicsAPI::Vulkan: return "Vulkan 1.4";
			}
		}

	private:
		static Platform::GraphicsAPI s_API;
	};

}
