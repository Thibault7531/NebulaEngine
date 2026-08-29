#include <CoreUtils.h>
#include <fstream>
#include <Platform/Nebula/CoreSystems/Events/WindowEvents.h>
#include <Platform/Nebula/CoreSystems/Windows/Window.h>
#include <Platform/Nebula/CoreSystems/Windows/WindowManager.h>

#include "Log.h"
#include "Metal3Surface.h"
#include "PlatformUtils.h"
#include "Metal/MTLRenderPass.hpp"
#include "QuartzCore/CAMetalDrawable.hpp"
#include "QuartzCore/CAMetalLayer.hpp"
#include "Metal/MTLCommandBuffer.hpp"
#include "Metal/MTLCommandQueue.hpp"
#include "Metal/MTLDevice.hpp"
#include "Metal/MTLLibrary.hpp"
#include "Metal/MTLRenderCommandEncoder.hpp"
#include "Metal/MTLRenderPipeline.hpp"
#include "Platform/Nebula/CoreSystems/Graphics/Surface.h"
#include "Platform/Nebula/CoreSystems/Graphics/GraphicsAPI.h"

struct Vertex
{
    float Position[3];
    float Color[3];
};

static bool s_Running = true;

bool OnWindowCloseEvent(const Nebula::Platform::WindowClosedEvent& event)
{
    s_Running = false;
    return true;
}

bool OnEvent(Nebula::Platform::Event& event)
{
    Nebula::Platform::EventDispatcher dispatcher{event};
    dispatcher.Dispatch<Nebula::Platform::WindowClosedEvent>(OnWindowCloseEvent);
}

int main()
{
    Nebula::Platform::Log::Init();
    Nebula::Platform::CoreUtils::InitThread();
    Nebula::Ref<Nebula::Platform::Window> window = Nebula::Platform::WindowManager::Get().CreateMainWindow({.API = Nebula::Platform::GraphicsAPI::Metal3});
    window->SetEventCallback(OnEvent);

    Nebula::Graphics::Surface* layer = window->GetSurface().get();
    Nebula::Graphics::Metal3Surface* metalContext = static_cast<Nebula::Graphics::Metal3Surface*>(layer);
    MTL::Device* metalDevice = MTL::CreateSystemDefaultDevice();
    MTL::CommandQueue* commandQueue = metalDevice->newCommandQueue();
    metalContext->SetDevice(metalDevice, commandQueue);

    Vertex vertices[3] = {
        {.Position = {-0.5f, -0.5f, 0.f}, .Color = {1.f, 0.f, 0.f}},
        {.Position = {0.f, 0.5f, 0.f}, .Color = {0.f, 1.f, 0.f}},
        {.Position = {0.5f, -0.5f, 0.f}, .Color = {0.f, 0.f, 1.f}},
    };

    MTL::Buffer* vertexBuffer = metalDevice->newBuffer(&vertices, sizeof(vertices), MTL::ResourceStorageModeShared);

    std::string shaderSource;
    std::ifstream shader(Nebula::Platform::Files::GetResourcesPath() / "assets/PlatformMetal3Example/shaders/triangle.metal", std::ios::in | std::ios::binary);
    if (shader)
    {
        shader.seekg(0, std::ios::end);
        shaderSource.resize(shader.tellg());
        shader.seekg(0, std::ios::beg);
        shader.read(&shaderSource[0], shaderSource.size());
        shader.close();
    }
    MTL::CompileOptions* compileOptions = MTL::CompileOptions::alloc()->init();
    compileOptions->setCompileSymbolVisibility(MTL::CompileSymbolVisibilityDefault);
    NS::Error* error;
    MTL::Library* library = metalDevice->newLibrary(NS::String::string(shaderSource.c_str(), NS::ASCIIStringEncoding), compileOptions, &error);
    if (!library)
    {
        NB_PLATFORM_ERROR("Could not compile Metal shader: {}", error->localizedDescription()->utf8String());
    }

    MTL::Function* vertexShader = library->newFunction(NS::String::string("vertexShader", NS::ASCIIStringEncoding));
    MTL::Function* fragmentShader = library->newFunction(NS::String::string("fragmentShader", NS::ASCIIStringEncoding));

    MTL::RenderPipelineDescriptor* renderPipelineDescriptor = MTL::RenderPipelineDescriptor::alloc()->init();
    renderPipelineDescriptor->setLabel(NS::String::string("Triangle shader", NS::ASCIIStringEncoding));
    renderPipelineDescriptor->setVertexFunction(vertexShader);
    renderPipelineDescriptor->setFragmentFunction(fragmentShader);
    MTL::PixelFormat pixelFormat = MTL::PixelFormatRGBA8Unorm;
    renderPipelineDescriptor->colorAttachments()->object(0)->setPixelFormat(pixelFormat);

    MTL::RenderPipelineState* renderPipelineState = metalDevice->newRenderPipelineState(renderPipelineDescriptor, &error);

    renderPipelineDescriptor->release();

    MTL::TextureDescriptor* textureDescriptor = MTL::TextureDescriptor::alloc()->init();
    textureDescriptor->setPixelFormat(MTL::PixelFormatRGBA8Unorm);
    textureDescriptor->setWidth(window->GetClientSize().first);
    textureDescriptor->setHeight(window->GetClientSize().second);
    textureDescriptor->setUsage(MTL::TextureUsageRenderTarget | MTL::TextureUsagePixelFormatView);

    MTL::Texture* texture = metalDevice->newTexture(textureDescriptor);
    textureDescriptor->release();

    while (s_Running)
    {
        NS::AutoreleasePool* pool = NS::AutoreleasePool::alloc()->init();

        metalContext->NewFrame();

        MTL::CommandBuffer* commandBuffer = commandQueue->commandBuffer();

        MTL::RenderPassDescriptor* renderPassDescriptor = MTL::RenderPassDescriptor::alloc()->init();
        renderPassDescriptor->colorAttachments()->object(0)->setLoadAction(MTL::LoadActionClear);
        renderPassDescriptor->colorAttachments()->object(0)->setStoreAction(MTL::StoreActionStore);
        renderPassDescriptor->colorAttachments()->object(0)->setClearColor(MTL::ClearColor(0.2f, 0.3f, 0.3f, 1));
        renderPassDescriptor->colorAttachments()->object(0)->setTexture(texture);

        MTL::RenderCommandEncoder* renderCommandEncoder = commandBuffer->renderCommandEncoder(renderPassDescriptor);
        renderCommandEncoder->setRenderPipelineState(renderPipelineState);
        renderCommandEncoder->setVertexBuffer(vertexBuffer, 0, 0);
        MTL::PrimitiveType typeTriangle = MTL::PrimitiveTypeTriangle;
        NS::UInteger vertexStart = 0;
        NS::UInteger vertexCount = 3;
        renderCommandEncoder->drawPrimitives(typeTriangle, vertexStart, vertexCount);
        renderCommandEncoder->endEncoding();

        commandBuffer->commit();
        commandBuffer->waitUntilCompleted();

        renderPassDescriptor->release();

        window->Present(texture);
        window->OnUpdate();
        
        pool->release();
    }

    window.reset();
}
