#include "OpenGLContext.h"

#include <glad/glad.h>

#include "Nebula/CoreSystems/Core/Core.h"

#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
		const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
	const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

typedef BOOL wglSwapIntervalEXT_type(int interval);
wglSwapIntervalEXT_type *wglSwapIntervalEXT;

typedef int wglGetSwapIntervalEXT_type();
wglGetSwapIntervalEXT_type *wglGetSwapIntervalEXT;

namespace Nebula {

	bool OpenGLContext::s_GLInitialized = false;

	OpenGLContext::OpenGLContext(const HWND windowHandle)
		: m_WindowHandle(windowHandle), m_DeviceContext(nullptr), m_Context(nullptr)
	{
	}

	OpenGLContext::~OpenGLContext()
	{
		wglDeleteContext(m_Context);
	}

	void OpenGLContext::Init()
	{
		NB_PROFILE_FUNCTION();

		if (!s_GLInitialized) InitOpenGL();

		int pixel_format_attribs[] = {
			WGL_DRAW_TO_WINDOW_ARB,     GL_TRUE,
			WGL_SUPPORT_OPENGL_ARB,     GL_TRUE,
			WGL_DOUBLE_BUFFER_ARB,      GL_TRUE,
			WGL_ACCELERATION_ARB,       WGL_FULL_ACCELERATION_ARB,
			WGL_PIXEL_TYPE_ARB,         WGL_TYPE_RGBA_ARB,
			WGL_COLOR_BITS_ARB,         32,
			WGL_DEPTH_BITS_ARB,         24,
			WGL_STENCIL_BITS_ARB,       8,
			0
		};

		m_DeviceContext = GetDC(m_WindowHandle);

		int pixel_format;
		UINT num_formats;
		wglChoosePixelFormatARB(m_DeviceContext, pixel_format_attribs, nullptr, 1, &pixel_format, &num_formats);

		PIXELFORMATDESCRIPTOR pfd;
		DescribePixelFormat(m_DeviceContext, pixel_format, sizeof(pfd), &pfd);
		SetPixelFormat(m_DeviceContext, pixel_format, &pfd);

		int gl46_attribs[] = {
			WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
			WGL_CONTEXT_MINOR_VERSION_ARB, 6,
			WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
			0,
		};

		m_Context = wglCreateContextAttribsARB(m_DeviceContext, s_GLInitialized ? static_cast<HGLRC>(GetMain()->GetContext()) : nullptr, gl46_attribs);

		wglMakeCurrent(m_DeviceContext, m_Context);

		if (!s_GLInitialized)
		{
			s_MainContext = this;
		}

		int status = gladLoadGL();
		NB_CORE_ASSERT(status, "Failed to initialize Glad!");

		s_GLInitialized = true;

	#ifdef NB_ENABLE_ASSERTS
		int versionMajor;
		int versionMinor;
		glGetIntegerv(GL_MAJOR_VERSION, &versionMajor);
		glGetIntegerv(GL_MINOR_VERSION, &versionMinor);

		NB_CORE_ASSERT(versionMajor > 4 || (versionMajor == 4 && versionMinor >= 6), "Nebula requires OpenGL version 4.6")
	#endif
	}

	void OpenGLContext::SetCurrent()
	{
		wglMakeCurrent(m_DeviceContext, m_Context);
	}

	void OpenGLContext::SwapBuffers()
	{
		::SwapBuffers(m_DeviceContext);
	}

	void OpenGLContext::SetVSync(bool enabled)
	{
		if (enabled) wglSwapIntervalEXT(1);
		else wglSwapIntervalEXT(0);
	}

	bool OpenGLContext::GetVSync()
	{
		return wglGetSwapIntervalEXT() == 1;
	}

	void OpenGLContext::InitOpenGL()
	{
		WNDCLASSA window_class = {
	        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
	        .lpfnWndProc = DefWindowProcA,
	        .hInstance = GetModuleHandle(nullptr),
	        .lpszClassName = "Dummy_WGL_djuasiodwa",
	    };

	    RegisterClassA(&window_class);

	    HWND dummy_window = CreateWindowExA(
	        0,
	        window_class.lpszClassName,
	        "Dummy OpenGL Window",
	        0,
	        CW_USEDEFAULT,
	        CW_USEDEFAULT,
	        CW_USEDEFAULT,
	        CW_USEDEFAULT,
	        nullptr,
	        nullptr,
	        window_class.hInstance,
	        nullptr);

	    HDC dummy_dc = GetDC(dummy_window);

	    PIXELFORMATDESCRIPTOR pfd = {
	        .nSize = sizeof(pfd),
	        .nVersion = 1,
	    	.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
	        .iPixelType = PFD_TYPE_RGBA,
	        .cColorBits = 32,
	        .cAlphaBits = 8,
	        .cDepthBits = 24,
	        .cStencilBits = 8,
	    	.iLayerType = PFD_MAIN_PLANE,
	    };

	    int pixel_format = ChoosePixelFormat(dummy_dc, &pfd);
	    SetPixelFormat(dummy_dc, pixel_format, &pfd);

	    HGLRC dummy_context = wglCreateContext(dummy_dc);

	    wglMakeCurrent(dummy_dc, dummy_context);

	    InitGLExtensions();

	    wglMakeCurrent(dummy_dc, nullptr);
	    wglDeleteContext(dummy_context);
	    ReleaseDC(dummy_window, dummy_dc);
	    DestroyWindow(dummy_window);
	}

	void OpenGLContext::InitGLExtensions()
	{
		wglCreateContextAttribsARB = reinterpret_cast<wglCreateContextAttribsARB_type*>(wglGetProcAddress("wglCreateContextAttribsARB"));
		wglChoosePixelFormatARB = reinterpret_cast<wglChoosePixelFormatARB_type*>(wglGetProcAddress("wglChoosePixelFormatARB"));
		wglSwapIntervalEXT = reinterpret_cast<wglSwapIntervalEXT_type*>(wglGetProcAddress("wglSwapIntervalEXT"));
		wglGetSwapIntervalEXT = reinterpret_cast<wglGetSwapIntervalEXT_type*>(wglGetProcAddress("wglGetSwapIntervalEXT"));
	}
}
