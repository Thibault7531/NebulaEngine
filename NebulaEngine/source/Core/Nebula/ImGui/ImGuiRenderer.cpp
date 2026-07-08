#include "ImGuiRenderer.h"

#include "Nebula/CoreSystems/Core/Log.h"
#include "Nebula/Renderer/Texture.h"
#include "Nebula/Renderer/Core/RenderCommand.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"

#define IMGUI_MAX_VTX_BUFFER_SIZE (512 * 1024)

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

        platform_io.Renderer_TextureMaxWidth = platform_io.Renderer_TextureMaxHeight = RenderCommand::GetMaxTextureSize();

        switch (RendererAPI::GetAPI())
        {
        case Platform::GraphicsAPI::None: break;
        case Platform::GraphicsAPI::OpenGL410:
            {
                renderer_data->HasPolygonMode = true;
                break;
            }
        case Platform::GraphicsAPI::OpenGL460:
            {
                renderer_data->HasPolygonMode = true;
                renderer_data->HasClipOrigin = true;
                break;
            }
        }

        platform_io.Renderer_RenderWindow = RenderWindow;
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

    void ImGuiRenderer::SetupRenderState(ImDrawData& drawData, uint32_t width, uint32_t height, Ref<VertexArray> vertexArray)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        renderer_data.BlendState = RenderCommand::GetBlendingState();
        renderer_data.isEnabledFaceCulling = RenderCommand::IsFaceCullingEnabled();
        renderer_data.isEnabledDepthTest = RenderCommand::IsDepthTestEnabled();
        renderer_data.isEnabledStencilTest = RenderCommand::IsStencilTestEnabled();
        renderer_data.isEnabledScissorTest = RenderCommand::IsScissorTestEnabled();
        renderer_data.isEnabledPrimitiveRestart = RenderCommand::IsPrimitiveRestartEnabled();
        renderer_data.PolygonMode = RenderCommand::GetPolygonMode();

        RendererAPI::BlendState blendState;
        blendState.enabled = true;
        blendState.srcColorFactor = RendererAPI::BlendFactor::SourceAlpha;
        blendState.dstColorFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
        blendState.srcAlphaFactor = RendererAPI::BlendFactor::One;
        blendState.dstAlphaFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
        RenderCommand::SetBlendingState(blendState);

        RenderCommand::EnableFaceCulling(false);
        RenderCommand::EnableDepthTest(false);
        RenderCommand::EnableStencilTest(false);
        RenderCommand::EnableScissorTest(true);
        RenderCommand::EnablePrimitiveRestart(false);
        if (renderer_data.HasPolygonMode) RenderCommand::SetPolygonMode(RendererAPI::PolygonMode::Fill);

        bool clipOriginLowerLeft = true;
        if (renderer_data.HasClipOrigin && RenderCommand::GetClipOrigin() == RendererAPI::ClipOrigin::UpperLeft) clipOriginLowerLeft = false;

        RenderCommand::SetViewport(0, 0, width, height);
        float L = drawData.DisplayPos.x;
        float R = drawData.DisplayPos.x + drawData.DisplaySize.x;
        float T = drawData.DisplayPos.y;
        float B = drawData.DisplayPos.y + drawData.DisplaySize.y;
        if (!clipOriginLowerLeft)
        {
            float tmp = T;
            T = B;
            B = tmp;
        }

        glm::mat4 ortho_projection {
            { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
        };

        renderer_data.shader->Bind();
        renderer_data.uniformBuffer->SetData(0, &ortho_projection);
        renderer_data.shader->SetUniformBlock("ImGuiUBO", renderer_data.uniformBuffer);
        renderer_data.shader->SetInt("u_Texture", 0);
        renderer_data.shader->Unbind();

        vertexArray->Bind();
        vertexArray->AddVertexBuffer(renderer_data.vertexBuffer);
        vertexArray->SetIndexBuffer(renderer_data.indexBuffer);
        vertexArray->Unbind();
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

        Ref<VertexArray> vertexArray = VertexArray::Create();
        SetupRenderState(*drawData, width, height, vertexArray);

        ImVec2 clip_off = drawData->DisplayPos;
        ImVec2 clip_scale = drawData->FramebufferScale;

        for (uint32_t n = 0; n < drawData->CmdListsCount; n++)
        {
            const ImDrawList* cmdList = drawData->CmdLists[n];

            const GLsizeiptr vtx_buffer_size = cmdList->VtxBuffer.Size * sizeof(ImDrawVert);
            const GLsizeiptr idx_buffer_size = cmdList->IdxBuffer.Size;

            renderer_data.vertexBuffer->SetData(cmdList->VtxBuffer.Data, vtx_buffer_size);
            renderer_data.indexBuffer->SetData(cmdList->IdxBuffer.Data, idx_buffer_size);

            for (uint32_t cmd_i = 0; cmd_i < cmdList->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd cmd = cmdList->CmdBuffer[cmd_i];
                if (cmd.UserCallback != nullptr)
                {
                    if (cmd.UserCallback == ImDrawCallback_ResetRenderState) SetupRenderState(*drawData, width, height, vertexArray);
                    else cmd.UserCallback(cmdList, &cmd);
                }
                else
                {
                    ImVec2 clip_min((cmd.ClipRect.x - clip_off.x) * clip_scale.x, (cmd.ClipRect.y - clip_off.y) * clip_scale.y);
                    ImVec2 clip_max((cmd.ClipRect.z - clip_off.x) * clip_scale.x, (cmd.ClipRect.w - clip_off.y) * clip_scale.y);
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y) continue;

                    renderer_data.shader->Bind();
                    RenderCommand::SetScissor(clip_min.x, height - clip_max.y, clip_max.x - clip_min.x, clip_max.y - clip_min.y);
                    Texture::BindWithID(cmd.GetTexID());
                    vertexArray->Bind();
                    RenderCommand::DrawIndexedVertexOffset(vertexArray, cmd.ElemCount, sizeof(ImDrawIdx) == 1 ? RendererAPI::DataType::UnsignedByte : sizeof(ImDrawIdx) == 2 ? RendererAPI::DataType::UnsignedShort : RendererAPI::DataType::UnsignedInt, cmd.IdxOffset * sizeof(ImDrawIdx), cmd.VtxOffset);
                    vertexArray->Unbind();
                    renderer_data.shader->Unbind();
                }
            }
        }

        UndoRenderState();
    }

    void ImGuiRenderer::UndoRenderState()
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        RenderCommand::SetBlendingState(renderer_data.BlendState);
        RenderCommand::EnableFaceCulling(renderer_data.isEnabledFaceCulling);
        RenderCommand::EnableDepthTest(renderer_data.isEnabledDepthTest);
        RenderCommand::EnableStencilTest(renderer_data.isEnabledStencilTest);
        RenderCommand::EnableScissorTest(renderer_data.isEnabledScissorTest);
        RenderCommand::EnablePrimitiveRestart(renderer_data.isEnabledPrimitiveRestart);
        if (renderer_data.HasPolygonMode) RenderCommand::SetPolygonMode(renderer_data.PolygonMode);
    }

    void ImGuiRenderer::CreateTexture(ImTextureData& textureData)
    {
        ImGuiIO& io = ImGui::GetIO();
        RendererData& renderer_data = *static_cast<RendererData*>(io.BackendRendererUserData);

        IM_ASSERT(textureData.Format == ImTextureFormat_RGBA32);

        Ref<Texture2D> texture = Texture2D::Create(textureData.Width, textureData.Height, {});
        renderer_data.textures.push_back(texture);
        texture->SetData(textureData.Pixels, textureData.Width * textureData.Height, Texture::DataFormat::RGBA);

        textureData.SetTexID(texture->GetRendererID());
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
            if (renderer_data.textures[i]->GetRendererID() == textureData.GetTexID())
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

        renderer_data.shader = Shader::Create(Platform::Files::GetResourcesPath() / "assets/NebulaEngine/shaders/ImGui.slang");
        renderer_data.uniformBuffer = UniformBuffer::Create({
            {ShaderDataType::Mat4, "u_ProjectionMatrix"}
        });
        renderer_data.vertexBuffer = VertexBuffer::Create(IMGUI_MAX_VTX_BUFFER_SIZE);
        renderer_data.vertexBuffer->SetLayout({
            { ShaderDataType::Float2, "a_Position" },
            { ShaderDataType::Float2, "a_UV" },
            { ShaderDataType::UByte4, "a_Color", true }
        });
        renderer_data.indexBuffer = IndexBuffer::Create();
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
        if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
        {
            RenderCommand::SetClearColor({0.f, 0.f, 0.f, 1.f});
            RenderCommand::Clear();
        }
        RenderDrawData(viewport->DrawData);
    }

}
