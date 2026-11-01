#include "ImGuiRenderer.h"

#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/CoreSystems/Core/Log.h"
#include "Nebula/Graphics/Texture.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"

namespace Nebula
{

    void ImGuiRenderer::Init()
    {
        NB_CORE_DEBUG("Initializing Nebula::ImGui::Renderer for ImGui");

        ImGuiIO& io = ImGui::GetIO();
        ImGuiPlatformIO& platform_io = ImGui::GetPlatformIO();


        // Set ImGui parameters
        RendererData* renderer_data = new RendererData();
        io.BackendRendererName = "Nebula::ImGui::Renderer";
        io.BackendRendererUserData = static_cast<void*>(renderer_data);

        io.BackendFlags |= ImGuiBackendFlags_RendererHasVtxOffset;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasTextures;
        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;

        platform_io.Renderer_TextureMaxWidth = platform_io.Renderer_TextureMaxHeight = Application::Get().GetDevice()->GetMaximumTextureSize();
        platform_io.Renderer_RenderWindow = RenderWindow;

        renderer_data->device = Application::Get().GetDevice();
    }

    void ImGuiRenderer::Shutdown()
    {
        ImGui::DestroyPlatformWindows();

        DestroyDeviceObjects();

        ImGuiIO& io = ImGui::GetIO();
        auto renderer_data = static_cast<RendererData*>(io.BackendRendererUserData);
        delete renderer_data;
        io.BackendRendererName = nullptr;
        io.BackendRendererUserData = nullptr;
        io.BackendFlags &= ~ImGuiBackendFlags_RendererHasVtxOffset;
        io.BackendFlags &= ~ImGuiBackendFlags_RendererHasTextures;
    }

    void ImGuiRenderer::BeginFrame()
    {
        ImGuiIO& io = ImGui::GetIO();
        auto renderer_data = static_cast<RendererData*>(io.BackendRendererUserData);

        if (!renderer_data->shader) CreateDeviceObjects();
    }

    void ImGuiRenderer::SetupRenderState(ImDrawData& drawData, uint32_t width, uint32_t height, Ref<Graphics::RenderPass> renderPass, Ref<Graphics::VertexArray> vertexArray)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        renderPass->SetCullMode(Graphics::CullMode::None);
        renderPass->SetDepthStencilState({.depthCompareFunction = Graphics::DepthCompareFunction::Always, .depthWrite = false});
        renderPass->SetViewport(0, 0, width, height, 0.f, 1.f);

        float L = drawData.DisplayPos.x;
        float R = drawData.DisplayPos.x + drawData.DisplaySize.x;
        float T = drawData.DisplayPos.y;
        float B = drawData.DisplayPos.y + drawData.DisplaySize.y;
        float N = 0.f;
        float F = 1.f;

        glm::mat4 ortho_projection {
            { 2.0f/(R-L),   0.0f,         0.0f,      0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,      0.0f },
            { 0.0f,         0.0f,         1/(F-N),   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  N/(F-N),   1.0f },
        };

        renderPass->SetShaderPipeline(renderer_data.shader);
        renderer_data.uniformBuffer->SetData(0, &ortho_projection);
        renderPass->SetUniformBlock("ImGuiUBO", renderer_data.uniformBuffer);

        vertexArray->AddVertexBuffer(renderer_data.vertexBuffer);
        vertexArray->SetIndexBuffer(renderer_data.indexBuffer);
    }

    void ImGuiRenderer::RenderDrawData(ImDrawData* drawData)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        uint32_t width = drawData->DisplaySize.x * drawData->FramebufferScale.x;
        uint32_t height = drawData->DisplaySize.y * drawData->FramebufferScale.y;

        if (drawData->Textures != nullptr)
        {
            for (ImTextureData* tex : *drawData->Textures)
            {
                if (tex->Status != ImTextureStatus_OK)
                {
                    UpdateTexture(*tex);
                }
            }
        }

        renderer_data.vertexBuffer = renderer_data.device->CreateVertexBuffer(drawData->TotalVtxCount > 0 ? drawData->TotalVtxCount * sizeof(ImDrawVert) : 4);
        renderer_data.vertexBuffer->SetLayout({
            {Graphics::ShaderDataType::Float2, "a_Position"},
            {Graphics::ShaderDataType::Float2, "a_UV"},
            {Graphics::ShaderDataType::UByte4, "a_Color"}
        });
        renderer_data.indexBuffer = renderer_data.device->CreateIndexBuffer(drawData->TotalIdxCount > 0 ? drawData->TotalIdxCount * sizeof(ImDrawIdx) : 3);
        Ref<Graphics::VertexArray> vertexArray = Graphics::VertexArray::Create();
        Ref<Graphics::CommandBuffer> commandBuffer = renderer_data.device->CreateCommandBuffer();
        ViewportData* viewport_data = static_cast<ViewportData*>(drawData->OwnerViewport->PlatformUserData);
        Ref<Graphics::RenderPass> renderPass = commandBuffer->CreateRenderPass(viewport_data->Swapchain->GetCurrentFrame(), false);
        SetupRenderState(*drawData, width, height, renderPass, vertexArray);

        ImVec2 clip_off = drawData->DisplayPos;
        ImVec2 clip_scale = drawData->FramebufferScale;

        uint32_t vtxOffset = 0;
        uint32_t idxOffset = 0;
        for (uint32_t n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmdList = drawData->CmdLists[n];

            const uint32_t vtx_buffer_size = cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
            const uint32_t idx_buffer_size = cmdList->IdxBuffer.Size;

            renderer_data.vertexBuffer->SetData(cmdList->VtxBuffer.Data, vtx_buffer_size, vtxOffset);
            renderer_data.indexBuffer->SetData((uint16_t*)cmdList->IdxBuffer.Data, idx_buffer_size, idxOffset);

            for (uint32_t cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* cmd = &cmdList->CmdBuffer[cmd_i];
                if (cmd->UserCallback != nullptr)
                {
                    if (cmd->UserCallback == ImDrawCallback_ResetRenderState) SetupRenderState(*drawData, width, height, renderPass, vertexArray);
                    else cmd->UserCallback(cmdList, cmd);
                }
                else
                {
                    ImVec2 clip_min((cmd->ClipRect.x - clip_off.x) * clip_scale.x, (cmd->ClipRect.y - clip_off.y) * clip_scale.y);
                    ImVec2 clip_max((cmd->ClipRect.z - clip_off.x) * clip_scale.x, (cmd->ClipRect.w - clip_off.y) * clip_scale.y);
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) continue;

                    renderPass->SetScissor(clip_min.x, clip_min.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);
                    renderPass->BindTextureToSlot(reinterpret_cast<Graphics::Texture2D*>(cmd->GetTexID()), 0);
                    renderPass->DrawArrayIndexedOffset(vertexArray, cmd->ElemCount, idxOffset + cmd->IdxOffset * sizeof(ImDrawIdx), vtxOffset/sizeof(ImDrawVert) + cmd->VtxOffset);
                }
            }

            vtxOffset += cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
            idxOffset += cmdList->IdxBuffer.Size * sizeof(ImDrawIdx);
        }

        renderPass->EndRenderPass();
        commandBuffer->Commit();
    }

    void ImGuiRenderer::CreateTexture(ImTextureData& textureData)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        IM_ASSERT(textureData.Format == ImTextureFormat_RGBA32);

        Ref<Graphics::Texture2D> texture = renderer_data.device->CreateTexture2D(textureData.Width, textureData.Height, {});
        renderer_data.textures.push_back(texture);
        texture->SetData(textureData.Pixels, textureData.Width * textureData.Height * textureData.BytesPerPixel, Graphics::Texture::DataFormat::RGBA);

        textureData.SetTexID(reinterpret_cast<uint64_t>(texture.get()));
        textureData.SetStatus(ImTextureStatus_OK);
    }

    void ImGuiRenderer::DestroyTexture(ImTextureData& textureData)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        uint32_t i = 0;
        bool found = false;
        while (i < renderer_data.textures.size() && !found)
        {
            if (renderer_data.textures[i].get() == reinterpret_cast<Graphics::Texture2D*>(textureData.GetTexID()))
            {
                found = true;
            }
            else
            {
                i++;
            }
        }
        if (found)
        {
            renderer_data.textures[i].reset();
            renderer_data.textures.erase(renderer_data.textures.begin() + i);
        }

        textureData.SetTexID(ImTextureID_Invalid);
        textureData.SetStatus(ImTextureStatus_Destroyed);
    }

    void ImGuiRenderer::UpdateTexture(ImTextureData& textureData)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        if (textureData.Status == ImTextureStatus_WantCreate)
        {
            IM_ASSERT(textureData.TexID == 0 && textureData.BackendUserData == nullptr);
            CreateTexture(textureData);
        }
        else if (textureData.Status == ImTextureStatus_WantUpdates)
        {
            DestroyTexture(textureData);
            CreateTexture(textureData);
        }
        else if (textureData.Status == ImTextureStatus_WantDestroy)
        {
            DestroyTexture(textureData);
        }
    }

    void ImGuiRenderer::CreateDeviceObjects()
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        renderer_data.shaderLibrary.Load(Platform::Files::GetResourcesPath() / "assets/NebulaEngine/shaders/ImGui.slang");
        Graphics::BlendState blendState;
        blendState.enabled = true;
        blendState.srcColorFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstColorFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        blendState.srcAlphaFactor = Graphics::BlendFactor::One;
        blendState.dstAlphaFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        Graphics::VertexLayout pipelineLayout = {
            { Graphics::ShaderDataType::Float2, "a_Position" },
            { Graphics::ShaderDataType::Float2, "a_UV" },
            { Graphics::ShaderDataType::UByte4, "a_Color" }
        };
        renderer_data.shader = renderer_data.shaderLibrary.CreatePipeline("ImGui", pipelineLayout, {.framebufferSpec = {}, .blendState = blendState});
        renderer_data.uniformBuffer = renderer_data.device->CreateUniformBuffer({
            {Graphics::ShaderDataType::Matrix4, "u_ProjectionMatrix"}
        });
    }

    void ImGuiRenderer::DestroyDeviceObjects()
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        renderer_data.shader.reset();
        renderer_data.uniformBuffer.reset();
        renderer_data.vertexBuffer.reset();
        renderer_data.indexBuffer.reset();
    }

    void ImGuiRenderer::RenderWindow(ImGuiViewport* viewport, void*)
    {
        RenderDrawData(viewport->DrawData);
    }

}
