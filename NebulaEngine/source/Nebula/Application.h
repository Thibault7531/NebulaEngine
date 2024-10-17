#pragma once

#include "Core.h"
#include "Events/Event.h"

namespace Nebula {

	class NEBULA_API Application
	{
	public:
		Application();
		virtual ~Application();
		
		void Run();
	};

	Application* CreateApplication();
}
