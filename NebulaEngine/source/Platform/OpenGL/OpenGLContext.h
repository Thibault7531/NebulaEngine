#pragma once

#include <Windows.h>

#include "Nebula/Renderer/Core/GraphicsContext.h"

namespace Nebula {

	class OpenGLContext : public GraphicsContext
	{
	public:
		OpenGLContext(HWND windowHandle);
		virtual ~OpenGLContext();

		virtual void Init() override;

		virtual void SetCurrent() override;
		virtual void SwapBuffers() override;

		virtual void SetVSync(bool enabled) override;
		virtual bool GetVSync() override;

		virtual void* GetContext() const override { return m_Context; }

	private:
		static void InitOpenGL();
		static void InitGLExtensions();

	private:
		HWND m_WindowHandle;
		HDC m_DeviceContext;
		HGLRC m_Context;

		static bool s_GLInitialized;
	};

}
