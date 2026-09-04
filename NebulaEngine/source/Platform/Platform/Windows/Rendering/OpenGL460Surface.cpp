#include "OpenGL460Surface.h"

#include <windows.ui.interop.h>
#include <glad/wgl.h>
#include <glad/gl.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"

namespace Nebula::Graphics
{
    bool OpenGL460Surface::s_GLInitialized = false;
    HGLRC OpenGL460Surface::s_MainContext = nullptr;

    OpenGL460Surface::OpenGL460Surface(Platform::Window* window)
        : m_Window(window)
    {
        //winrt::Microsoft::UI::Windowing::AppWindow* nativeWindow = static_cast<winrt::Microsoft::UI::Windowing::AppWindow*>(window->GetNativeWindow());
        HRESULT hr = GetWindowFromWindowId({window->GetID()}, &m_Handle);
        if (hr != S_OK) NB_PLATFORM_CRITICAL("Can't get HWND for AppWindow");

        m_DeviceContext = GetDC(m_Handle);

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

    OpenGL460Surface::~OpenGL460Surface()
    {
        wglMakeCurrent(nullptr, nullptr);
        wglDeleteContext(m_Context);
        ReleaseDC(m_Handle, m_DeviceContext);
    }

    std::string OpenGL460Surface::GetAPIVersion() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VERSION));
    }

    std::string OpenGL460Surface::GetAPIVendor() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_VENDOR));
    }

    std::string OpenGL460Surface::GetAPIRenderer() const
    {
        return reinterpret_cast<const char*>(glGetString(GL_RENDERER));
    }

    void OpenGL460Surface::SetVSync(VSyncMode mode)
    {
        switch (mode)
        {
        case VSyncMode::Off: wglSwapIntervalEXT(0); break;
        case VSyncMode::On: wglSwapIntervalEXT(1); break;
        case VSyncMode::Adaptive: wglSwapIntervalEXT(-1); break;
        }
    }

    VSyncMode OpenGL460Surface::IsVsync() const
    {
        int result = wglGetSwapIntervalEXT();

        if (result == -1) return VSyncMode::Adaptive;
        return result ? VSyncMode::On : VSyncMode::Off;
    }

    void OpenGL460Surface::NewFrame()
    {
        wglMakeCurrent(m_DeviceContext, m_Context);
    }

    void OpenGL460Surface::Present(void* texture)
    {
        NewFrame();
        GLuint fbID = static_cast<GLuint>(reinterpret_cast<intptr_t>(texture));
        if (fbID != 0) glBlitNamedFramebuffer(fbID, 0, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, 0, 0, m_Window->GetSize().first, m_Window->GetSize().second, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        ::SwapBuffers(m_DeviceContext);
    }

    void OpenGL460Surface::InitOpenGL()
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
