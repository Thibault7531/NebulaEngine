#pragma once

#include <imgui.h>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Graphics/RenderPass.h"
#include "Nebula/Graphics/ShaderModule.h"
#include "Nebula/Graphics/ShaderPipeline.h"
#include "Nebula/Graphics/Texture.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"

namespace Nebula
{
    struct RendererData
    {
        Ref<Graphics::Device> device;
        Graphics::ShaderModuleLibrary shaderLibrary;
        Ref<Graphics::ShaderPipeline> shader;
        Ref<Graphics::UniformBuffer> uniformBuffer;

        uint32_t currentFrameIndex = 0;
        std::vector<Ref<Graphics::CommandBuffer>> commandBuffers;
        std::vector<Ref<Graphics::VertexBuffer>> vertexBuffers;
        std::vector<Ref<Graphics::IndexBuffer>> indexBuffers;
        std::vector<Ref<Graphics::Texture>> textures;
    };

    class ImGuiRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();

        static void SetupRenderState(ImDrawData& drawData, uint32_t width, uint32_t height, Ref<Graphics::RenderPass> renderPass, Ref<Graphics::VertexArray> vertexArray);
        static void RenderDrawData(ImDrawData* drawData);

        static void CreateTexture(ImTextureData& textureData);
        static void DestroyTexture(ImTextureData& textureData);
        static void UpdateTexture(ImTextureData& textureData);

        static void CreateDeviceObjects();
        static void DestroyDeviceObjects();

        static void RenderWindow(ImGuiViewport* viewport, void*);
    };

}
