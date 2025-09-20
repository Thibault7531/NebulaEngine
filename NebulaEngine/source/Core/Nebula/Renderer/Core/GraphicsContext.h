#pragma once

#include "Nebula/CoreSystems/Core/Core.h"

namespace Nebula {

	class GraphicsContext
	{
	public:
		virtual ~GraphicsContext() = default;

		virtual void Init() = 0;

		virtual void SetCurrent() = 0;
		virtual void SwapBuffers() = 0;

		virtual void SetVSync(bool enabled) = 0;
		virtual bool GetVSync() = 0;

		virtual void* GetContext() const = 0;

	public:
		static GraphicsContext* GetMain() { return s_MainContext; }

	protected:
		static GraphicsContext* s_MainContext;
	};

}