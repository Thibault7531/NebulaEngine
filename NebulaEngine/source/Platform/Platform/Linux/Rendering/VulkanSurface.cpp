#define VULKAN_HPP_STORAGE_SHARED 1
#define VULKAN_HPP_STORAGE_SHARED_EXPORT 1
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC 1
#define VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

#include "VulkanSurface.h"

#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "SDL3/SDL_vulkan.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace Nebula::Graphics
{
    VulkanSurface::VulkanSurface(Platform::Window* window)
        : m_Window(window) {}

    VulkanSurface::~VulkanSurface()
    {
        SDL_Vulkan_DestroySurface(m_Instance, m_Surface, nullptr);
    }

    std::string VulkanSurface::GetAPIVersion() const
    {
        if (m_PhysicalDevice)
        {
            vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();
            uint32_t version = properties.apiVersion;

            uint32_t major = VK_API_VERSION_MAJOR(version);
            uint32_t minor = VK_API_VERSION_MINOR(version);
            uint32_t patch = VK_API_VERSION_PATCH(version);

            return "Vulkan " + std::to_string(major) + "." + std::to_string(minor) + "." + std::to_string(patch);
        }
        return "";
    }

    std::string VulkanSurface::GetAPIVendor() const
    {
        if (m_PhysicalDevice)
        {
            vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();
            uint32_t vendorID = properties.vendorID;

            if (vendorID == 0x10DE) return "NVIDIA";
            if (vendorID == 0x1002) return "AMD";
            if (vendorID == 0x8086) return "Intel";
            if (vendorID == 0x13B5) return "ARM";
            if (vendorID == 0x5143) return "Qualcomm";
            if (vendorID == 0x106B) return "Apple";
        }
        return "";
    }

    std::string VulkanSurface::GetAPIRenderer() const
    {
        if (m_PhysicalDevice)
        {
            vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();
            return properties.deviceName;
        }
        return "";
    }

    void VulkanSurface::SetInstance(void* instance)
    {
        m_Instance = *static_cast<vk::Instance*>(instance);
        VkInstance vkInstance = *static_cast<VkInstance*>(instance);
    }

    void VulkanSurface::SetDevice(void* physicalDevice, void* logicalDevice, void* commandQueue)
    {
        m_PhysicalDevice = *static_cast<vk::PhysicalDevice*>(physicalDevice);
        m_LogicalDevice = *static_cast<vk::Device*>(logicalDevice);

        if (!SDL_Vulkan_CreateSurface(static_cast<SDL_Window*>(m_Window->GetNativeWindow()), m_Instance, nullptr, &m_Surface))
        {
            NB_PLATFORM_CRITICAL("Failed to create Vulkan surface: {}", SDL_GetError());
        }

        m_SurfaceCapabilities = m_PhysicalDevice.getSurfaceCapabilitiesKHR(m_Surface);

        vk::Extent2D swapchainExtent = m_SurfaceCapabilities.currentExtent;
        if (m_SurfaceCapabilities.currentExtent.width == 0xFFFFFFFF)
        {
            swapchainExtent.width = m_Window->GetClientSize().first;
            swapchainExtent.height = m_Window->GetClientSize().second;
        }
    }
}
