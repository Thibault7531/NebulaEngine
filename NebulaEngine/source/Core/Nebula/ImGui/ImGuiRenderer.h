#pragma once

#include <imgui.h>

#include "Nebula/CoreSystems/Core/Core.h"
#include "Nebula/Renderer/Shader.h"
#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/Buffers/VertexArray.h"
#include "Nebula/Renderer/Core/RendererAPI.h"

namespace Nebula
{
    struct RendererData
    {
        bool HasPolygonMode = false;
        bool HasClipOrigin = false;

        Ref<Shader> shader;
        Ref<UniformBuffer> uniformBuffer;
        Ref<VertexBuffer> vertexBuffer;
        Ref<IndexBuffer> indexBuffer;

        std::vector<Ref<Texture>> textures;

        // Last states
        RendererAPI::BlendState BlendState;
        bool isEnabledFaceCulling;
        bool isEnabledDepthTest;
        bool isEnabledStencilTest;
        bool isEnabledScissorTest;
        bool isEnabledPrimitiveRestart;
        RendererAPI::PolygonMode PolygonMode;
    };

    class ImGuiRenderer
    {
    public:
        static void Init();
        static void Shutdown();

        static void BeginFrame();

        static void SetupRenderState(ImDrawData& drawData, uint32_t width, uint32_t height, Ref<VertexArray> vertexArray);
        static void RenderDrawData(ImDrawData* drawData);
        static void UndoRenderState();

        static void CreateTexture(ImTextureData& textureData);
        static void DestroyTexture(ImTextureData& textureData);
        static void UpdateTexture(ImTextureData& textureData);

        static void CreateDeviceObjects();
        static void DestroyDeviceObjects();

        static void RenderWindow(ImGuiViewport* viewport, void*);
    };

}
