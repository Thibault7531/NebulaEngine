#pragma once
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Platform/Nebula/CoreSystems/Windows/Window.h"
#include "vulkan/vulkan.hpp"

namespace Nebula::Graphics
{
    class VulkanSurface : public Surface
    {
    public:
        VulkanSurface(Platform::Window* window);
        virtual ~VulkanSurface();

        virtual std::string GetAPIVersion() const override;
        virtual std::string GetAPIVendor() const override;
        virtual std::string GetAPIRenderer() const override;

        virtual void SetInstance(void* instance) override;
        virtual void SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue) override;
        virtual void* GetSurface() const override;

    private:
        Platform::Window* m_Window;
        vk::Instance m_Instance;
        vk::PhysicalDevice m_PhysicalDevice;
        vk::Device m_LogicalDevice;

        VkSurfaceKHR m_Surface;
        VkSurfaceCapabilitiesKHR m_SurfaceCapabilities;

        static bool s_IsInitialized;
    };
}
