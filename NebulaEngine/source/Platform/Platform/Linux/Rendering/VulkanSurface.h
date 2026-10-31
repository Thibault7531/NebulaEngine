#pragma once

#include <vulkan/vulkan.hpp>

#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"

namespace Nebula::Graphics
{
    class VulkanSurface : public Surface
    {
    public:
        VulkanSurface(Platform::Window* window);
        ~VulkanSurface() override;

        virtual std::string GetAPIVersion() const override;
        virtual std::string GetAPIVendor() const override;
        virtual std::string GetAPIRenderer() const override;

        virtual void SetInstance(void* instance) override;
        virtual void SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue) override;
        virtual void* GetSurface() const override { return m_Surface; }

        virtual void SetVSync(VSyncMode mode) override;
        virtual VSyncMode IsVsync() const override;
        virtual void Resize(uint32_t width, uint32_t height) override;

        virtual void NewFrame() override;
        virtual void Present(void* texture) override;

    public:
        vk::SurfaceKHR GetVkSurface() const { return m_Surface; }

    private:
        Platform::Window* m_Window;
        vk::Instance m_Instance;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;

        VkSurfaceKHR m_Surface;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities{};
    };
}
