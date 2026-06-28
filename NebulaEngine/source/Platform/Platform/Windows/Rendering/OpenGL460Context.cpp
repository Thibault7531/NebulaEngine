#include "OpenGL460Context.h"

#include <windows.ui.interop.h>
#include <glad/wgl.h>
#include <glad/gl.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Platform
{
    bool OpenGL460Context::s_GLInitialized = false;
    HGLRC OpenGL460Context::s_MainContext = nullptr;

    OpenGL460Context::OpenGL460Context(Window* window)
        : m_Window(window)
    {
        //winrt::Microsoft::UI::Windowing::AppWindow* nativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(window->GetNativeWindow());
        HRESULT hr = GetWindowFromWindowId({window->GetID()}, &m_Handle);
        if (hr != S_OK) NB_PLATFORM_CRITICAL("Can't get HWND for AppWindow");

        m_DeviceContext = GetDC(m_Handle);
    }

    OpenGL460Context::~OpenGL460Context()
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_Context);
        ReleaseDC(m_Handle, m_DeviceContext);
    }

    void OpenGL460Context::Init()
    {
        if (!s_GLInitialized) InitOpenGL();

        int attributes[] = {
            WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
            WGL_CONTEXT_MINOR_VERSION_ARB, 6,
            WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
            WGL_CONTEXT_PROFILE_MASK_ARB,  WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
            0
        };

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
        int pixel_format = ChoosePixelFormat(m_DeviceContext, &pfd);
        SetPixelFormat(m_DeviceContext, pixel_format, &pfd);

        m_Context = wglCreateContextAttribsARB(m_DeviceContext, s_MainContext, attributes);
        if (!m_Context)
        {
            NB_PLATFORM_ERROR("Could not create OpenGL Context: {}", GetLastError());
        }
        if (!s_MainContext) s_MainContext = m_Context;
        wglMakeCurrent(m_DeviceContext, m_Context);

        gladLoaderLoadGL();
    }

    void OpenGL460Context::SetCurrent()
    {
        wglMakeCurrent(m_DeviceContext, m_Context);
    }

    void OpenGL460Context::SetVSync(VSyncMode mode)
    {
        switch (mode)
        {
        case VSyncMode::Off: wglSwapIntervalEXT(0); break;
        case VSyncMode::On: wglSwapIntervalEXT(1); break;
        case VSyncMode::Adaptive: wglSwapIntervalEXT(-1); break;
        }
    }

    VSyncMode OpenGL460Context::IsVsync() const
    {
        int result = wglGetSwapIntervalEXT();

        if (result == -1) return VSyncMode::Adaptive;
        return result ? VSyncMode::On : VSyncMode::Off;
    }

    void OpenGL460Context::SwapBuffers()
    {
        ::SwapBuffers(m_DeviceContext);
    }

    void OpenGL460Context::Present(uint32_t framebuffer)
    {
        glBlitNamedFramebuffer(framebuffer, 0, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        SwapBuffers();
    }

    void OpenGL460Context::InitOpenGL()
    {
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
        int pixel_format = ChoosePixelFormat(m_DeviceContext, &pfd);
        SetPixelFormat(m_DeviceContext, pixel_format, &pfd);

        HGLRC gl_context = wglCreateContext(m_DeviceContext);
        wglMakeCurrent(m_DeviceContext, gl_context);
        gladLoaderLoadWGL(m_DeviceContext);
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(gl_context);

        s_GLInitialized = true;
    }
}
