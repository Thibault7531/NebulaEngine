#define VMA_IMPLEMENTATION

#include "VulkanDevice.h"

#include <vulkan/vulkan.hpp>

#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"
#include "vk_mem_alloc.hpp"
#include "volk/volk.h"

namespace Nebula::Graphics
{
    bool VulkanDevice::s_InstanceInitialized = false;
    vk::Instance VulkanDevice::s_Instance;

    VulkanDevice::VulkanDevice()
    {
        if (!s_InstanceInitialized)
        {
            volkInitialize();

            VkApplicationInfo applicationInfo;
            applicationInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
            applicationInfo.pApplicationName = Platform::Files::GetAppName().c_str();
            applicationInfo.pEngineName = "NebulaEngine";
            applicationInfo.apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0);

            std::vector<const char*> extensions = Platform::Graphics::getRequiredInstanceExtensions(Platform::GraphicsAPI::Vulkan);

            const std::vector<const char*> validationLayers = {
                "VK_LAYER_KHRONOS_validation"
            };
            uint32_t layerCount = 0;
            vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
            std::vector<VkLayerProperties> layerProperties(layerCount);
            vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());

            bool validationLayerEnabled = false;
#ifdef NB_ENGINE_DEBUG
            validationLayerEnabled = true;
            for (auto& validationLayer : validationLayers)
            {
                bool layerFound = false;
                for (auto& layerProperties : layerProperties)
                {
                    if (strcmp(validationLayer, layerProperties.layerName) == 0)
                    {
                        layerFound = true;
                        break;
                    }
                }
                validationLayerEnabled &= layerFound;
            }

            if (validationLayerEnabled)
            {
                extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
            else
            {
                NB_CORE_CRITICAL("Validation layer not found");
            }
#endif

            VkInstanceCreateInfo instanceCreateInfo;
            instanceCreateInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
            instanceCreateInfo.pApplicationInfo = &applicationInfo;
            instanceCreateInfo.enabledLayerCount = validationLayerEnabled ? validationLayers.size() : 0;
            instanceCreateInfo.ppEnabledLayerNames = validationLayerEnabled ? validationLayers.data() : nullptr;
            instanceCreateInfo.enabledExtensionCount = extensions.size();
            instanceCreateInfo.ppEnabledExtensionNames = extensions.data();

            VkInstance cInstance;
            vkCreateInstance(&instanceCreateInfo, nullptr, &cInstance);
            s_Instance = cInstance;

            volkLoadInstance(s_Instance);
            VULKAN_HPP_DEFAULT_DISPATCHER.init(s_Instance, vkGetInstanceProcAddr);
        }

        auto physicalDevices = s_Instance.enumeratePhysicalDevices();
        m_PhysicalDevice = physicalDevices[0];

        auto queueFamilies = m_PhysicalDevice.getQueueFamilyProperties();
        for (uint32_t i = 0; i < queueFamilies.size(); i++)
        {
            if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
            {
                m_GraphicsQueueFamily = i;
                break;
            }
        }
        const float qfPriorities = 1.0f;
        vk::DeviceQueueCreateInfo queueCreateInfo;
        queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
        queueCreateInfo.queueFamilyIndex = m_GraphicsQueueFamily;
        queueCreateInfo.queueCount = 1;
        queueCreateInfo.pQueuePriorities = &qfPriorities;

        const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

        vk::PhysicalDeviceVulkan12Features enabledVK12Features;
        enabledVK12Features.sType = vk::StructureType::ePhysicalDeviceVulkan12Features;
        enabledVK12Features.descriptorIndexing = true;
        enabledVK12Features.shaderSampledImageArrayNonUniformIndexing = true;
        enabledVK12Features.descriptorBindingVariableDescriptorCount = true;
        enabledVK12Features.runtimeDescriptorArray = true;
        enabledVK12Features.bufferDeviceAddress = true;

        vk::PhysicalDeviceVulkan13Features enabledVK13Features;
        enabledVK13Features.sType = vk::StructureType::ePhysicalDeviceVulkan13Features;
        enabledVK13Features.pNext = &enabledVK12Features;
        enabledVK13Features.synchronization2 = true;
        enabledVK13Features.dynamicRendering = true;

        vk::PhysicalDeviceFeatures enabledVKFeatures;
        enabledVKFeatures.samplerAnisotropy = true;

        vk::DeviceCreateInfo deviceCreateInfo;
        deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
        deviceCreateInfo.pNext = &enabledVK13Features;
        deviceCreateInfo.queueCreateInfoCount = 1;
        deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
        deviceCreateInfo.enabledExtensionCount = deviceExtensions.size();
        deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
        deviceCreateInfo.pEnabledFeatures = &enabledVKFeatures;
        m_LogicalDevice = m_PhysicalDevice.createDevice(deviceCreateInfo);

        m_GraphicsQueue = m_LogicalDevice.getQueue(m_GraphicsQueueFamily, 0);

        vma::VulkanFunctions vkFunctions;
        vkFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
        vkFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
        vkFunctions.vkCreateImage = vkCreateImage;

        vma::AllocatorCreateInfo allocatorCreateInfo;
        allocatorCreateInfo.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;
        allocatorCreateInfo.physicalDevice = m_PhysicalDevice;
        allocatorCreateInfo.device = m_LogicalDevice;
        allocatorCreateInfo.pVulkanFunctions = &vkFunctions;
        allocatorCreateInfo.instance = s_Instance;
        m_Allocator = vma::createAllocator(allocatorCreateInfo);
    }

    VulkanDevice::~VulkanDevice()
    {
        m_Allocator.destroy();
        m_GraphicsQueue = nullptr;
        m_LogicalDevice.destroy();
        m_PhysicalDevice = nullptr;
    }

    void VulkanDevice::SetSurfaceDevice(Ref<Surface> surface)
    {
        surface->SetInstance(&s_Instance);
        surface->SetDevice(&m_PhysicalDevice, &m_LogicalDevice, nullptr);
    }

    Ref<VertexBuffer> VulkanDevice::CreateVertexBuffer(uint32_t size) {}
    Ref<VertexBuffer> VulkanDevice::CreateVertexBuffer(const void* data, uint32_t size) {}
    Ref<IndexBuffer> VulkanDevice::CreateIndexBuffer(uint32_t count) {}
    Ref<IndexBuffer> VulkanDevice::CreateIndexBuffer(uint16_t* data, uint32_t count) {}
    Ref<IndexBuffer> VulkanDevice::CreateIndexBuffer(uint32_t* data, uint32_t count) {}
    Ref<UniformBuffer> VulkanDevice::CreateUniformBuffer(const BufferLayout& layout, uint32_t instances) {}

    Ref<Texture2D> VulkanDevice::CreateTexture2D(uint32_t width, uint32_t height,
        const Texture::TextureSpecification& spec) {}

    Ref<Texture2D> VulkanDevice::
    CreateTexture2D(std::filesystem::path path, const Texture::TextureSpecification& spec) {}

    Ref<Framebuffer> VulkanDevice::CreateFramebuffer(const FramebufferSpecification& spec) {}
    Ref<Shader> VulkanDevice::CreateShader(const std::filesystem::path& path) {}

    Ref<ShaderPipeline> VulkanDevice::CreateShaderPipeline(Ref<Shader> shader, VertexLayout layout,
        const PipelineSpecification& spec) {}

    Ref<CommandBuffer> VulkanDevice::CreateCommandBuffer() {}
    uint32_t VulkanDevice::GetMaximumTextureSize() {}
    uint32_t VulkanDevice::GetUniformBufferAlignment() {}
}
