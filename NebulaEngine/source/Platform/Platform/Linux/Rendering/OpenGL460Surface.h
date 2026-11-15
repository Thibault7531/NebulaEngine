#pragma once

#include <SDL3/SDL.h>

#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Graphics
{

    class OpenGL460Surface : public Surface
    {
    public:
        OpenGL460Surface(Platform::Window* window);
        virtual ~OpenGL460Surface();

        virtual std::string GetAPIVersion() const override;
        virtual std::string GetAPIVendor() const override;
        virtual std::string GetAPIRenderer() const override;

        virtual void SetInstance(void* instance) override {}
        virtual void SetDevice(void* device, void* logicalDevice, void* commandQueue) override {}
        virtual void* GetSurface() const override { return m_Context; }

    private:
        Platform::Window* m_Window;
        SDL_GLContext m_Context = nullptr;

    private:
        static bool s_GLInitialized;
        static SDL_GLContext s_MainContext;
    };

}
