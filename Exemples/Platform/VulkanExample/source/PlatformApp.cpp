#define VMA_IMPLEMENTATION

#include <slang-com-ptr.h>
#include <slang.h>
#include <volk.h>
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

#include "Platform/Nebula/CoreSystems/Core/CoreUtils.h"
#include "Platform/Nebula/CoreSystems/Core/Log.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"
#include "Platform/Nebula/CoreSystems/Events/KeyEvent.h"
#include "Platform/Nebula/CoreSystems/Events/WindowEvents.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

static bool s_running = true;
static bool s_RecreateSwapchain = false;

bool OnWindowResized(Nebula::Platform::WindowResizedEvent event)
{
    s_RecreateSwapchain = true;

    return true;
}

void Callback(Nebula::Platform::Event& event)
{
    if (event.GetEventType() == Nebula::Platform::EventType::WindowClosed) s_running = false;

    Nebula::Platform::EventDispatcher dispatcher(event);
    dispatcher.Dispatch<Nebula::Platform::WindowResizedEvent>(OnWindowResized);
}

void chk(vk::Result result)
{
    if (result != vk::Result::eSuccess)
    {
        NB_PLATFORM_CRITICAL("Vulkan error: {}", static_cast<int>(result));
    }
}

void chk(VkResult result)
{
    if (result != VK_SUCCESS)
    {
        NB_PLATFORM_CRITICAL("Vulkan error: {}", static_cast<int>(result));
    }
}

int main()
{
    Nebula::Platform::Log::Init();
    Nebula::Platform::CoreUtils::InitThread();

    Nebula::Platform::WindowProperties windowProperties;
    windowProperties.API = Nebula::Platform::GraphicsAPI::Vulkan;
    windowProperties.Title = "Platform Vulkan Example";
    windowProperties.Style = Nebula::Platform::WindowStyle::Default;
    Nebula::Ref<Nebula::Platform::Window> window = Nebula::Platform::WindowManager::Get().CreateMainWindow(windowProperties);
    window->SetEventCallback(Callback);

    VkApplicationInfo appInfo{
        .sType = VK_STRUCTURE_TYPE_APPLICATION_INFO,
        .pApplicationName = NB_PLATFORM_APP_NAME,
        .pEngineName = "Nebula::Platform",
        .apiVersion = VK_MAKE_API_VERSION(0, 1, 4, 0),
    };

    std::vector<const char*> instanceExtensions = Nebula::Platform::Graphics::getRequiredInstanceExtensions(Nebula::Platform::GraphicsAPI::Vulkan);

    const std::vector<const char*> validationLayers = {
        "VK_LAYER_KHRONOS_validation"
    };
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);
    std::vector<VkLayerProperties> layerProperties(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, layerProperties.data());
    bool validationLayersEnabled = false;
    for (auto const& layer : layerProperties)
    {
        if (strcmp(layer.layerName, "VK_LAYER_KHRONOS_validation") == 0)
        {
            validationLayersEnabled = true;
        }
    }
    if (!validationLayersEnabled)
    {
        NB_PLATFORM_CRITICAL("Validation layers are not available");
    }
    else
    {
        instanceExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    VkInstanceCreateInfo instanceCreateInfo{
        .sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO,
        .pApplicationInfo = &appInfo,
        .enabledLayerCount = static_cast<uint32_t>(validationLayers.size()),
        .ppEnabledLayerNames = validationLayers.data(),
        .enabledExtensionCount = static_cast<uint32_t>(instanceExtensions.size()),
        .ppEnabledExtensionNames = instanceExtensions.data(),
    };

    VkInstance instance;
    chk(vkCreateInstance(&instanceCreateInfo, nullptr, &instance));

    window->GetSurface()->SetInstance(&instance);
    vk::Instance cppInstance(instance);

    auto physicalDevices = cppInstance.enumeratePhysicalDevices();
    vk::PhysicalDeviceProperties2 deviceProperties = physicalDevices[0].getProperties2();
    NB_PLATFORM_INFO("Physical device name: {}", deviceProperties.properties.deviceName.data());

    auto queueFamilies = physicalDevices[0].getQueueFamilyProperties();
    uint32_t queueFamily = 0;
    for (uint32_t i = 0; i < queueFamilies.size(); i++)
    {
        if (queueFamilies[i].queueFlags & vk::QueueFlagBits::eGraphics)
        {
            queueFamily = i;
            break;
        }
    }
    const float qfPriorities = 1.0f;
    vk::DeviceQueueCreateInfo queueCreateInfo;
    queueCreateInfo.sType = vk::StructureType::eDeviceQueueCreateInfo;
    queueCreateInfo.queueFamilyIndex = queueFamily;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &qfPriorities;

    const std::vector<const char*> deviceExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };

    vk::PhysicalDeviceVulkan12Features enabledVk12Features;
    enabledVk12Features.sType = vk::StructureType::ePhysicalDeviceVulkan12Features;
    enabledVk12Features.descriptorIndexing = true;
    enabledVk12Features.shaderSampledImageArrayNonUniformIndexing = true;
    enabledVk12Features.descriptorBindingVariableDescriptorCount = true;
    enabledVk12Features.runtimeDescriptorArray = true;
    enabledVk12Features.bufferDeviceAddress = true;
    vk::PhysicalDeviceVulkan13Features enabledVk13Features;
    enabledVk13Features.sType = vk::StructureType::ePhysicalDeviceVulkan13Features;
    enabledVk13Features.pNext = &enabledVk12Features;
    enabledVk13Features.synchronization2 = true;
    enabledVk13Features.dynamicRendering = true;
    vk::PhysicalDeviceFeatures enabledVk10Features;
    enabledVk10Features.samplerAnisotropy = true;

    vk::DeviceCreateInfo deviceCreateInfo;
    deviceCreateInfo.sType = vk::StructureType::eDeviceCreateInfo;
    deviceCreateInfo.pNext = &enabledVk13Features;
    deviceCreateInfo.queueCreateInfoCount = 1;
    deviceCreateInfo.pQueueCreateInfos = &queueCreateInfo;
    deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
    deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();
    deviceCreateInfo.pEnabledFeatures = &enabledVk10Features;
    vk::Device device = physicalDevices[0].createDevice(deviceCreateInfo);

    vk::Queue queue = device.getQueue(queueFamily, 0);

    vma::VulkanFunctions vkFunctions;
    vkFunctions.vkGetInstanceProcAddr = vkGetInstanceProcAddr;
    vkFunctions.vkGetDeviceProcAddr = vkGetDeviceProcAddr;
    vkFunctions.vkCreateImage = vkCreateImage;
    vma::AllocatorCreateInfo allocatorCreateInfo;
    allocatorCreateInfo.flags = vma::AllocatorCreateFlagBits::eBufferDeviceAddress;
    allocatorCreateInfo.physicalDevice = physicalDevices[0];
    allocatorCreateInfo.device = device;
    allocatorCreateInfo.pVulkanFunctions = &vkFunctions;
    allocatorCreateInfo.instance = instance;
    vma::Allocator allocator = vma::createAllocator(allocatorCreateInfo);

    NB_PLATFORM_INFO("Successfully created Vulkan device and allocator");

    Nebula::Ref<Nebula::Graphics::Surface> surface = std::dynamic_pointer_cast<Nebula::Graphics::Surface>(window->GetSurface());
    surface->SetDevice(&physicalDevices[0], &device, &queue);
    vk::SurfaceKHR surfaceKHR = static_cast<VkSurfaceKHR>(surface->GetSurface());

    vk::SurfaceCapabilitiesKHR surfaceCapabilities = physicalDevices[0].getSurfaceCapabilitiesKHR(surfaceKHR);
    vk::Extent2D swapchainExtent = surfaceCapabilities.currentExtent;
    if (swapchainExtent.width == 0xFFFFFFFF)
    {
        swapchainExtent.width = window->GetClientSize().first;
        swapchainExtent.height = window->GetClientSize().second;
    }
    const vk::Format imageFormat = vk::Format::eB8G8R8A8Unorm;
    vk::SwapchainCreateInfoKHR swapchainCreateInfo;
    swapchainCreateInfo.sType = vk::StructureType::eSwapchainCreateInfoKHR;
    swapchainCreateInfo.surface = surfaceKHR;
    swapchainCreateInfo.minImageCount = surfaceCapabilities.minImageCount;
    swapchainCreateInfo.imageFormat = imageFormat;
    swapchainCreateInfo.imageColorSpace = vk::ColorSpaceKHR::eSrgbNonlinear;
    swapchainCreateInfo.imageExtent = swapchainExtent;
    swapchainCreateInfo.imageArrayLayers = 1;
    swapchainCreateInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;
    swapchainCreateInfo.preTransform = vk::SurfaceTransformFlagBitsKHR::eIdentity;
    swapchainCreateInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
    swapchainCreateInfo.presentMode = vk::PresentModeKHR::eFifo;
    vk::SwapchainKHR swapchain = device.createSwapchainKHR(swapchainCreateInfo);

    std::vector<vk::Image> swapchainImages = device.getSwapchainImagesKHR(swapchain);
    std::vector<vk::ImageView> swapchainImageViews(swapchainImages.size());
    for (uint32_t imageIndex = 0; imageIndex < swapchainImageViews.size(); imageIndex++)
    {
        vk::ImageViewCreateInfo imageViewCreateInfo;
        imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
        imageViewCreateInfo.image = swapchainImages[imageIndex];
        imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
        imageViewCreateInfo.format = vk::Format::eB8G8R8A8Unorm;
        imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        swapchainImageViews[imageIndex] = device.createImageView(imageViewCreateInfo);
    }

    const uint32_t maxFramesInFlight = surfaceCapabilities.maxImageCount ? std::max(surfaceCapabilities.maxImageCount, 3u) : 3;
    uint32_t frameInFlightIndex = 0;

    vk::SemaphoreCreateInfo semaphoreCreateInfo;
    semaphoreCreateInfo.sType = vk::StructureType::eSemaphoreCreateInfo;
    vk::FenceCreateInfo fenceCreateInfo;
    fenceCreateInfo.sType = vk::StructureType::eFenceCreateInfo;
    fenceCreateInfo.flags = vk::FenceCreateFlagBits::eSignaled;
    std::vector<vk::Fence> fences(maxFramesInFlight);
    std::vector<vk::Semaphore> imageAcquiredSemaphores(maxFramesInFlight);
    std::vector<vk::Semaphore> renderCompleteSemaphores(maxFramesInFlight);
    for (uint32_t i = 0; i < maxFramesInFlight; i++)
    {
        fences[i] = device.createFence(fenceCreateInfo);
        imageAcquiredSemaphores[i] = device.createSemaphore(semaphoreCreateInfo);
        renderCompleteSemaphores[i] = device.createSemaphore(semaphoreCreateInfo);
    }

    const float vertices[] = {
        -0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f,
    };
    const uint32_t indices[] = {
        0, 1, 2
    };
    vk::DeviceSize vBufSize{ sizeof(vertices) };
    vk::DeviceSize iBufSize{ sizeof(indices) };
    vk::BufferCreateInfo bufferCreateInfo;
    bufferCreateInfo.sType = vk::StructureType::eBufferCreateInfo;
    bufferCreateInfo.size = vBufSize + iBufSize;
    bufferCreateInfo.usage = vk::BufferUsageFlagBits::eVertexBuffer | vk::BufferUsageFlagBits::eIndexBuffer;
    vma::AllocationCreateInfo vBufferAllocCreateInfo;
    vBufferAllocCreateInfo.flags = vma::AllocationCreateFlagBits::eHostAccessSequentialWrite | vma::AllocationCreateFlagBits::eHostAccessAllowTransferInstead | vma::AllocationCreateFlagBits::eMapped;
    vBufferAllocCreateInfo.usage = vma::MemoryUsage::eAuto;
    vma::AllocationInfo allocInfo;
    auto allocResult = allocator.createBuffer(bufferCreateInfo, vBufferAllocCreateInfo, allocInfo);
    memcpy(allocInfo.pMappedData, vertices, vBufSize);
    memcpy(static_cast<char*>(allocInfo.pMappedData) + vBufSize, indices, iBufSize);

    vk::CommandPoolCreateInfo commandPoolCreateInfo;
    commandPoolCreateInfo.sType = vk::StructureType::eCommandPoolCreateInfo;
    commandPoolCreateInfo.flags = vk::CommandPoolCreateFlagBits::eResetCommandBuffer;
    commandPoolCreateInfo.queueFamilyIndex = queueFamily;
    vk::CommandPool commandPool = device.createCommandPool(commandPoolCreateInfo);

    vk::CommandBufferAllocateInfo commandBufferAllocateInfo;
    commandBufferAllocateInfo.sType = vk::StructureType::eCommandBufferAllocateInfo;
    commandBufferAllocateInfo.commandPool = commandPool;
    commandBufferAllocateInfo.commandBufferCount = maxFramesInFlight;
    std::vector<vk::CommandBuffer> commandBuffers = device.allocateCommandBuffers(commandBufferAllocateInfo);

    Slang::ComPtr<slang::IGlobalSession> globalSession;
    slang::createGlobalSession(globalSession.writeRef());
    auto slangTargets{ std::to_array<slang::TargetDesc>({
        {
            .format{SLANG_SPIRV},
            .profile{globalSession->findProfile("spirv_1_4")}
        } })};
    auto slangOptions{ std::to_array<slang::CompilerOptionEntry>({
        {
            slang::CompilerOptionName::EmitSpirvDirectly,
            {slang::CompilerOptionValueKind::Int, 1}
        }})};
    slang::SessionDesc sessionDesc;
    sessionDesc.targets = slangTargets.data();
    sessionDesc.targetCount = slangTargets.size();
    sessionDesc.defaultMatrixLayoutMode = SLANG_MATRIX_LAYOUT_COLUMN_MAJOR;
    sessionDesc.compilerOptionEntries = slangOptions.data();
    sessionDesc.compilerOptionEntryCount = slangOptions.size();
    Slang::ComPtr<slang::ISession> session;
    globalSession->createSession(sessionDesc, session.writeRef());

    Slang::ComPtr<slang::IModule> module{
        session->loadModuleFromSource("triangle", "assets/PlatformVulkanExample/triangle.slang", nullptr)
    };
    Slang::ComPtr<ISlangBlob> spirv;
    module->getTargetCode(0, spirv.writeRef());

    vk::ShaderModuleCreateInfo shaderModuleCreateInfo;
    shaderModuleCreateInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
    shaderModuleCreateInfo.codeSize = spirv->getBufferSize();
    shaderModuleCreateInfo.pCode = reinterpret_cast<const uint32_t*>(spirv->getBufferPointer());
    vk::ShaderModule shaderModule = device.createShaderModule(shaderModuleCreateInfo);

    vk::PushConstantRange pushConstantRange;
    pushConstantRange.stageFlags = vk::ShaderStageFlagBits::eVertex;
    pushConstantRange.size = sizeof(vk::DeviceAddress);
    vk::PipelineLayoutCreateInfo pipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
    pipelineLayoutCreateInfo.setLayoutCount = 0;
    pipelineLayoutCreateInfo.pSetLayouts = nullptr;
    pipelineLayoutCreateInfo.pushConstantRangeCount = 1;
    pipelineLayoutCreateInfo.pPushConstantRanges = &pushConstantRange;
    vk::PipelineLayout pipelineLayout = device.createPipelineLayout(pipelineLayoutCreateInfo);

    vk::VertexInputBindingDescription vertexBinding;
    vertexBinding.binding = 0;
    vertexBinding.stride = sizeof(float) * 6;
    vertexBinding.inputRate = vk::VertexInputRate::eVertex;

    std::vector<vk::VertexInputAttributeDescription> vertexAttributes;
    vertexAttributes.push_back({ 0, 0, vk::Format::eR32G32B32Sfloat,  0 });
    vertexAttributes.push_back({ 1, 0, vk::Format::eR32G32B32Sfloat,  sizeof(float) * 3 });

    vk::PipelineVertexInputStateCreateInfo vertexInputStateCreateInfo;
    vertexInputStateCreateInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;
    vertexInputStateCreateInfo.vertexBindingDescriptionCount = 1;
    vertexInputStateCreateInfo.pVertexBindingDescriptions = &vertexBinding;
    vertexInputStateCreateInfo.vertexAttributeDescriptionCount = vertexAttributes.size();
    vertexInputStateCreateInfo.pVertexAttributeDescriptions = vertexAttributes.data();

    vk::PipelineInputAssemblyStateCreateInfo inputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
    inputAssemblyStateCreateInfo.topology = vk::PrimitiveTopology::eTriangleList;

    std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
    shaderStages.push_back(vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags(0), vk::ShaderStageFlagBits::eVertex, shaderModule, "vertexMain"});
    shaderStages.push_back(vk::PipelineShaderStageCreateInfo{vk::PipelineShaderStageCreateFlags(0), vk::ShaderStageFlagBits::eFragment, shaderModule, "fragmentMain"});

    vk::PipelineViewportStateCreateInfo viewportStateCreateInfo;
    viewportStateCreateInfo.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
    viewportStateCreateInfo.viewportCount = 1;
    viewportStateCreateInfo.scissorCount = 1;
    std::vector<vk::DynamicState> dynamicStates{vk::DynamicState::eViewport, vk::DynamicState::eScissor};
    vk::PipelineDynamicStateCreateInfo dynamicStateCreateInfo;
    dynamicStateCreateInfo.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
    dynamicStateCreateInfo.dynamicStateCount = dynamicStates.size();
    dynamicStateCreateInfo.pDynamicStates = dynamicStates.data();

    vk::PipelineRenderingCreateInfo renderingCreateInfo;
    renderingCreateInfo.sType = vk::StructureType::ePipelineRenderingCreateInfo;
    renderingCreateInfo.colorAttachmentCount = 1;
    renderingCreateInfo.pColorAttachmentFormats = &imageFormat;

    vk::PipelineColorBlendAttachmentState blendAttachment;
    blendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
    vk::PipelineColorBlendStateCreateInfo colorBlendStateCreateInfo;
    colorBlendStateCreateInfo.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
    colorBlendStateCreateInfo.attachmentCount = 1;
    colorBlendStateCreateInfo.pAttachments = &blendAttachment;
    vk::PipelineRasterizationStateCreateInfo rasterizationStateCreateInfo;
    rasterizationStateCreateInfo.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
    rasterizationStateCreateInfo.lineWidth = 1.0f;
    vk::PipelineMultisampleStateCreateInfo multisampleStateCreateInfo;
    multisampleStateCreateInfo.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
    multisampleStateCreateInfo.rasterizationSamples = vk::SampleCountFlagBits::e1;

    vk::GraphicsPipelineCreateInfo pipelineCreateInfo;
    pipelineCreateInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
    pipelineCreateInfo.pNext = &renderingCreateInfo;
    pipelineCreateInfo.stageCount = 2;
    pipelineCreateInfo.pStages = shaderStages.data();
    pipelineCreateInfo.pVertexInputState = &vertexInputStateCreateInfo;
    pipelineCreateInfo.pInputAssemblyState = &inputAssemblyStateCreateInfo;
    pipelineCreateInfo.pViewportState = &viewportStateCreateInfo;
    pipelineCreateInfo.pRasterizationState = &rasterizationStateCreateInfo;
    pipelineCreateInfo.pMultisampleState = &multisampleStateCreateInfo;
    pipelineCreateInfo.pColorBlendState = &colorBlendStateCreateInfo;
    pipelineCreateInfo.pDynamicState = &dynamicStateCreateInfo;
    pipelineCreateInfo.layout = pipelineLayout;
    vk::Pipeline pipeline = device.createGraphicsPipeline(nullptr, pipelineCreateInfo).value;

    while (s_running)
    {
        window->OnUpdate();

        if (s_RecreateSwapchain)
        {
            s_RecreateSwapchain = false;
            device.waitIdle();
            physicalDevices[0].getSurfaceCapabilitiesKHR(surfaceKHR, &surfaceCapabilities);
            swapchainCreateInfo.oldSwapchain = swapchain;
            swapchainCreateInfo.imageExtent.width = window->GetClientSize().first;
            swapchainCreateInfo.imageExtent.height = window->GetClientSize().second;
            device.createSwapchainKHR(&swapchainCreateInfo, nullptr, &swapchain);
            swapchainImages = device.getSwapchainImagesKHR(swapchain);
            for (auto& semaphore : renderCompleteSemaphores)
            {
                device.destroySemaphore(semaphore);
                device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphore);
            }
            for (auto& semaphore : imageAcquiredSemaphores)
            {
                device.destroySemaphore(semaphore);
                device.createSemaphore(&semaphoreCreateInfo, nullptr, &semaphore);
            }
            for (uint32_t imageIndex = 0; imageIndex < swapchainImageViews.size(); imageIndex++)
            {
                device.destroyImageView(swapchainImageViews[imageIndex]);
                vk::ImageViewCreateInfo imageViewCreateInfo;
                imageViewCreateInfo.sType = vk::StructureType::eImageViewCreateInfo;
                imageViewCreateInfo.image = swapchainImages[imageIndex];
                imageViewCreateInfo.viewType = vk::ImageViewType::e2D;
                imageViewCreateInfo.format = vk::Format::eB8G8R8A8Unorm;
                imageViewCreateInfo.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
                swapchainImageViews[imageIndex] = device.createImageView(imageViewCreateInfo);
            }
            device.destroySwapchainKHR(swapchainCreateInfo.oldSwapchain);
        }

        vk::Result result = device.waitForFences(1, &fences[frameInFlightIndex], true, 0);
        if (result != vk::Result::eSuccess)
        {
            continue;
        }
        uint32_t imageIndex;
        result = device.acquireNextImageKHR(swapchain, 16, imageAcquiredSemaphores[frameInFlightIndex], nullptr, &imageIndex);
        if (result == vk::Result::eErrorOutOfDateKHR)
        {
            s_RecreateSwapchain = true;
            continue;
        }
        if (result == vk::Result::eTimeout)
        {
            continue;
        }
        if (result == vk::Result::eSuboptimalKHR)
        {
            s_RecreateSwapchain = true;
            continue;
        }
        device.resetFences(1, &fences[frameInFlightIndex]);

        commandBuffers[frameInFlightIndex].reset();
        vk::CommandBufferBeginInfo beginInfo;
        beginInfo.sType = vk::StructureType::eCommandBufferBeginInfo;
        beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
        commandBuffers[frameInFlightIndex].begin(beginInfo);

        vk::ImageMemoryBarrier2 imageBarrier;
        imageBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
        imageBarrier.image = swapchainImages[imageIndex];
        imageBarrier.subresourceRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
        imageBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imageBarrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imageBarrier.srcAccessMask = vk::AccessFlagBits2::eNone;
        imageBarrier.dstAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        imageBarrier.oldLayout = vk::ImageLayout::eUndefined;
        imageBarrier.newLayout = vk::ImageLayout::eColorAttachmentOptimal;

        vk::DependencyInfo imageBarrierDependencyInfo;
        imageBarrierDependencyInfo.sType = vk::StructureType::eDependencyInfo;
        imageBarrierDependencyInfo.imageMemoryBarrierCount = 1;
        imageBarrierDependencyInfo.pImageMemoryBarriers = &imageBarrier;
        commandBuffers[frameInFlightIndex].pipelineBarrier2(imageBarrierDependencyInfo);

        vk::RenderingAttachmentInfo attachmentInfo;
        attachmentInfo.sType = vk::StructureType::eRenderingAttachmentInfo;
        attachmentInfo.imageView = swapchainImageViews[imageIndex];
        attachmentInfo.imageLayout = vk::ImageLayout::eColorAttachmentOptimal;
        attachmentInfo.loadOp = vk::AttachmentLoadOp::eClear;
        attachmentInfo.storeOp = vk::AttachmentStoreOp::eStore;
        attachmentInfo.clearValue = vk::ClearColorValue(std::array<float, 4>{0.2f, 0.3f, 0.3f, 1.0f});

        vk::RenderingInfo renderingInfo;
        renderingInfo.sType = vk::StructureType::eRenderingInfo;
        renderingInfo.renderArea = vk::Rect2D(vk::Offset2D(0, 0), vk::Extent2D(swapchainCreateInfo.imageExtent.width, swapchainCreateInfo.imageExtent.height));
        renderingInfo.layerCount = 1;
        renderingInfo.colorAttachmentCount = 1;
        renderingInfo.pColorAttachments = &attachmentInfo;
        commandBuffers[frameInFlightIndex].beginRendering(renderingInfo);

        vk::Viewport viewport;
        viewport.x = 0.0f;
        viewport.y = static_cast<float>(window->GetClientSize().second);
        viewport.width = static_cast<float>(window->GetClientSize().first);
        viewport.height = -static_cast<float>(window->GetClientSize().second);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        commandBuffers[frameInFlightIndex].setViewport(0, 1, &viewport);
        vk::Rect2D scissorRect;
        scissorRect.offset = vk::Offset2D(0, 0);
        scissorRect.extent = vk::Extent2D(window->GetClientSize().first, window->GetClientSize().second);
        commandBuffers[frameInFlightIndex].setScissor(0, 1, &scissorRect);

        commandBuffers[frameInFlightIndex].bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
        vk::DeviceSize offset = 0;
        commandBuffers[frameInFlightIndex].bindVertexBuffers(0, 1, &allocResult.second, &offset);
        commandBuffers[frameInFlightIndex].bindIndexBuffer(allocResult.second, vBufSize, vk::IndexType::eUint32);

        commandBuffers[frameInFlightIndex].drawIndexed(3, 1, 0, 0, 0);

        commandBuffers[frameInFlightIndex].endRendering();

        vk::ImageMemoryBarrier2 imagePresentBarrier;
        imagePresentBarrier.sType = vk::StructureType::eImageMemoryBarrier2;
        imagePresentBarrier.srcStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imagePresentBarrier.srcAccessMask = vk::AccessFlagBits2::eColorAttachmentWrite;
        imagePresentBarrier.dstStageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;
        imagePresentBarrier.dstAccessMask = vk::AccessFlagBits2::eNone;
        imagePresentBarrier.oldLayout = vk::ImageLayout::eColorAttachmentOptimal;
        imagePresentBarrier.newLayout = vk::ImageLayout::ePresentSrcKHR;
        imagePresentBarrier.image = swapchainImages[imageIndex];
        imagePresentBarrier.subresourceRange = {vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1};

        vk::DependencyInfo imagePresentDependencyInfo;
        imagePresentDependencyInfo.sType = vk::StructureType::eDependencyInfo;
        imagePresentDependencyInfo.imageMemoryBarrierCount = 1;
        imagePresentDependencyInfo.pImageMemoryBarriers = &imagePresentBarrier;
        commandBuffers[frameInFlightIndex].pipelineBarrier2(imagePresentDependencyInfo);

        commandBuffers[frameInFlightIndex].end();

        vk::SemaphoreSubmitInfo imageAcquiredSemaphoreSubmitInfo;
        imageAcquiredSemaphoreSubmitInfo.sType = vk::StructureType::eSemaphoreSubmitInfo;
        imageAcquiredSemaphoreSubmitInfo.semaphore = imageAcquiredSemaphores[frameInFlightIndex];
        imageAcquiredSemaphoreSubmitInfo.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

        vk::CommandBufferSubmitInfo submitInfo;
        submitInfo.sType = vk::StructureType::eCommandBufferSubmitInfo;
        submitInfo.commandBuffer = commandBuffers[frameInFlightIndex];

        vk::SemaphoreSubmitInfo renderCompletetSemaphoreSubmitInfo2;
        renderCompletetSemaphoreSubmitInfo2.sType = vk::StructureType::eSemaphoreSubmitInfo;
        renderCompletetSemaphoreSubmitInfo2.semaphore = renderCompleteSemaphores[frameInFlightIndex];
        renderCompletetSemaphoreSubmitInfo2.stageMask = vk::PipelineStageFlagBits2::eColorAttachmentOutput;

        vk::SubmitInfo2 submitInfo2;
        submitInfo2.sType = vk::StructureType::eSubmitInfo2;
        submitInfo2.waitSemaphoreInfoCount = 1;
        submitInfo2.pWaitSemaphoreInfos = &imageAcquiredSemaphoreSubmitInfo;
        submitInfo2.commandBufferInfoCount = 1;
        submitInfo2.pCommandBufferInfos = &submitInfo;
        submitInfo2.signalSemaphoreInfoCount = 1;
        submitInfo2.pSignalSemaphoreInfos = &renderCompletetSemaphoreSubmitInfo2;

        queue.submit2(1, &submitInfo2, fences[frameInFlightIndex]);

        vk::PresentInfoKHR presentInfo;
        presentInfo.sType = vk::StructureType::ePresentInfoKHR;
        presentInfo.waitSemaphoreCount = 1;
        presentInfo.pWaitSemaphores = &renderCompleteSemaphores[frameInFlightIndex];
        presentInfo.swapchainCount = 1;
        presentInfo.pSwapchains = &swapchain;
        presentInfo.pImageIndices = &imageIndex;

        queue.presentKHR(&presentInfo);

        frameInFlightIndex = (frameInFlightIndex + 1) % maxFramesInFlight;

        window->Present(0);
    }

    return 0;
}
