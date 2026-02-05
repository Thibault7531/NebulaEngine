#include "OpenGLContext.h"

#include <glad/gl.h>
#include <KHR/khrplatform.h>

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Platform/Linux/Windows/LinuxWindow.h"

namespace Nebula::Platform
{
    bool OpenGlContext::s_GLInitialized = false;
    EGLContext OpenGlContext::s_MainContext = nullptr;

    OpenGlContext::OpenGlContext(Window* window)
    {
        auto* native_window = dynamic_cast<LinuxWindow*>(window);

        if (!s_GLInitialized)
        {
            m_Display = eglGetDisplay(native_window->GetWaylandDisplay());
            EGLint major, minor;
            eglInitialize(m_Display, &major, &minor);
            if (major != 1 || minor != 5) NB_PLATFORM_ERROR("Wrong EGL version: {}.{}", major, minor);
            eglBindAPI(EGL_OPENGL_API);
        }
        else
        {
            m_Display = eglGetDisplay(native_window->GetWaylandDisplay());
        }

        EGLint attrib_list[] = {
            EGL_RED_SIZE, 8,
            EGL_GREEN_SIZE, 8,
            EGL_BLUE_SIZE, 8,
            EGL_ALPHA_SIZE, 8,
            EGL_DEPTH_SIZE, 24,
            EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
            EGL_RENDERABLE_TYPE, EGL_OPENGL_BIT,
            EGL_NONE
        };
        EGLConfig* configs;
        EGLint num_config;
        eglGetConfigs(m_Display, nullptr, 0, &num_config);
        if (num_config == 0) NB_PLATFORM_ERROR("Could not find a suitable EGLConfig");
        configs = static_cast<EGLConfig*>(malloc(num_config * sizeof(EGLConfig)));
        eglChooseConfig(m_Display, attrib_list, configs, num_config, &num_config);
        m_Config = configs[0];

        m_NativeWindow = wl_egl_window_create(static_cast<wl_surface*>(native_window->GetNativeWindow()), window->GetSize().first, window->GetSize().second);
        if (m_NativeWindow == EGL_NO_SURFACE) NB_PLATFORM_ERROR("Could not create WL_EGL window");
        EGLint surfaceAttribList[] = { EGL_NONE };
        m_Surface = eglCreateWindowSurface(m_Display, m_Config, reinterpret_cast<NativeWindowType>(m_NativeWindow), surfaceAttribList);
        if (!m_Surface)
        {
            switch (eglGetError())
            {
            case EGL_BAD_ALLOC:
                NB_PLATFORM_ERROR("Could not allocate EGL surface");
                break;

            default: break;
            }
        }

        const EGLint attrib_list2[] = {
            EGL_CONTEXT_CLIENT_VERSION, 4,
            EGL_NONE
        };
        m_Context = eglCreateContext(m_Display, m_Config, EGL_NO_CONTEXT, attrib_list2);
        if (m_Context == EGL_NO_CONTEXT) NB_PLATFORM_ERROR("Could not create OpenGL context: {}", eglGetError());
        eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);

        free(configs);

            if (!s_GLInitialized)
        {
            if (!gladLoadGL(eglGetProcAddress)) NB_PLATFORM_ERROR("Could not load OpenGL");
        }
    }

    OpenGlContext::~OpenGlContext()
    {
    }

    void OpenGlContext::Init()
    {
    }

    void OpenGlContext::SetCurrent()
    {
        eglMakeCurrent(m_Display, m_Surface, m_Surface, m_Context);
    }

    void OpenGlContext::SetVSync(VSyncMode mode)
    {
    }

    VSyncMode OpenGlContext::IsVsync()
    {
    }

    void OpenGlContext::SwapBuffers()
    {
        eglSwapBuffers(m_Display, m_Surface);
    }
}
