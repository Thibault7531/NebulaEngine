#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Nebula/Graphics/ShaderModule.h"
#include "Nebula/Graphics/Buffers/VertexArray.h"
#include "Cameras/Camera.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/Graphics/Device.h"
#include "Platform/Nebula/CoreSystems/Core/PlatformUtils.h"

namespace Nebula
{
    struct QuadVertex
    {
        glm::vec3 Position;
        glm::vec4 Color;
        float TexIndex;
        glm::vec2 TexCoord;
        float TilingFactor;
        int32_t entityID;
    };

    struct Renderer2DData
    {
        static constexpr uint32_t MaxQuads = 10000;
        static constexpr uint32_t MaxVertices = MaxQuads * 4;
        static constexpr uint32_t MaxIndices = MaxQuads * 6;
        static constexpr uint32_t MaxTextureSlots = 15;

        Ref<Graphics::Device> Device;
        Graphics::ShaderModuleLibrary ShaderLibrary;
        Ref<Graphics::VertexArray> QuadVertexArray;
        Ref<Graphics::VertexBuffer> QuadVertexBuffer;
        Ref<Graphics::ShaderPipeline> QuadShader;
        Ref<Graphics::UniformBuffer> QuadUniformBuffer;
        Ref<Graphics::Texture2D> WhiteTexture;

        Ref<Graphics::CommandBuffer> CommandBuffer;
        Ref<Graphics::RenderPass> RenderPass;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Graphics::Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        Renderer2D::Statistics Stats;

        bool CullingEnabled = false;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        NB_PROFILE_FUNCTION();

        s_Data.Device = Application::Get().GetDevice();

        s_Data.QuadVertexArray = Graphics::VertexArray::Create();

        s_Data.QuadVertexBuffer = s_Data.Device->CreateVertexBuffer(Renderer2DData::MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            { Graphics::ShaderDataType::Float3, "a_Position"},
            { Graphics::ShaderDataType::Float4, "a_Color"},
            { Graphics::ShaderDataType::Float, "a_TexIndex"},
            { Graphics::ShaderDataType::Float2, "a_TexCoord"},
            { Graphics::ShaderDataType::Float, "a_TilingFactor"},
            { Graphics::ShaderDataType::Int, "a_EntityID"}
        });
        s_Data.QuadVertexArray->AddVertexBuffer(s_Data.QuadVertexBuffer);

        s_Data.QuadVertexBufferBase = new QuadVertex[Renderer2DData::MaxVertices];

        auto* quadIndices = new uint32_t[Renderer2DData::MaxIndices];

        uint32_t offset = 0;
        for (uint32_t i = 0; i < Renderer2DData::MaxIndices; i += 6)
        {
            quadIndices[i + 0] = offset + 0;
            quadIndices[i + 1] = offset + 1;
            quadIndices[i + 2] = offset + 2;

            quadIndices[i + 3] = offset + 2;
            quadIndices[i + 4] = offset + 3;
            quadIndices[i + 5] = offset + 0;

            offset += 4;
        }

        Ref<Graphics::IndexBuffer> indexBuffer = s_Data.Device->CreateIndexBuffer(quadIndices, Renderer2DData::MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
        delete[] quadIndices;

        s_Data.ShaderLibrary.Load(Platform::Files::GetResourcesPath() / "assets/NebulaEngine/shaders/Renderer2D.slang");

        s_Data.QuadUniformBuffer = s_Data.Device->CreateUniformBuffer({
            { Graphics::ShaderDataType::Matrix4, "u_ViewProjection" },
            { Graphics::ShaderDataType::Float, "u_TilingFactor" }
        });

        s_Data.WhiteTexture = s_Data.Device->CreateTexture2D(1, 1, Graphics::Texture::InternalFormat::RGBA8);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t), Graphics::Texture::DataFormat::RGBA);

        s_Data.TextureSlots[0] = s_Data.WhiteTexture;
    }

    void Renderer2D::Shutdown()
    {
        NB_PROFILE_FUNCTION();

        s_Data.QuadVertexArray.reset();
        s_Data.QuadVertexBuffer.reset();
        s_Data.QuadUniformBuffer.reset();
        s_Data.QuadShader.reset();
        s_Data.WhiteTexture.reset();
        s_Data.ShaderLibrary.Clear();

        delete s_Data.QuadVertexBufferBase;

        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
            s_Data.TextureSlots[i].reset();
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        NB_PROFILE_FUNCTION();

        s_Data.CommandBuffer = s_Data.Device->GetGraphicsQueue()->CreateCommandBuffer();
        s_Data.RenderPass = s_Data.CommandBuffer->CreateRenderPass(Application::Get().GetMainWindowSwapchain()->GetCurrentFrame(), false);

        Graphics::VertexLayout layout = {
            { Graphics::ShaderDataType::Float3, "a_Position" },
            { Graphics::ShaderDataType::Float4, "a_Color" },
            { Graphics::ShaderDataType::Float, "a_TexIndex" },
            { Graphics::ShaderDataType::Float2, "a_TexCoord" },
            { Graphics::ShaderDataType::Float, "a_TilingFactor" },
            { Graphics::ShaderDataType::Int, "a_EntityID" }
        };
        Graphics::BlendState blendState;
        blendState.enabled = true;
        blendState.srcColorFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstColorFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        blendState.srcAlphaFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstAlphaFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        Graphics::PipelineSpecification spec;
        spec.stages = {
            { Graphics::ShaderStage::Vertex, s_Data.ShaderLibrary.Get("Renderer2D"), "vertexMain" },
            { Graphics::ShaderStage::Fragment, s_Data.ShaderLibrary.Get("Renderer2D"), "fragmentMain" }
        };
        spec.framebufferSpec = Application::Get().GetMainWindowSwapchain()->GetCurrentFrame()->GetSpecification();
        spec.blendState = blendState;
        s_Data.QuadShader = Application::Get().GetDevice()->CreateShaderPipeline(layout, spec);

        s_Data.RenderPass->SetCullMode(Graphics::CullMode::None)
            .SetDepthStencilState({.depthCompareFunction = Graphics::DepthCompareFunction::Less, .depthWrite = true});
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(camera.getViewProjectionMatrix()));

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        BeginScene(camera, transform, Application::Get().GetMainWindowSwapchain()->GetCurrentFrame(), false);
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform,
        Ref<Graphics::Framebuffer> framebuffer, bool clear)
    {
        NB_PROFILE_FUNCTION();

        s_Data.CommandBuffer = s_Data.Device->GetGraphicsQueue()->CreateCommandBuffer();
        s_Data.RenderPass = s_Data.CommandBuffer->CreateRenderPass(framebuffer, clear);

        Graphics::VertexLayout layout = {
            { Graphics::ShaderDataType::Float3, "a_Position" },
            { Graphics::ShaderDataType::Float4, "a_Color" },
            { Graphics::ShaderDataType::Float, "a_TexIndex" },
            { Graphics::ShaderDataType::Float2, "a_TexCoord" },
            { Graphics::ShaderDataType::Float, "a_TilingFactor" },
            { Graphics::ShaderDataType::Int, "a_EntityID" }
        };
        Graphics::BlendState blendState;
        blendState.enabled = true;
        blendState.srcColorFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstColorFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        blendState.srcAlphaFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstAlphaFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        Graphics::PipelineSpecification spec;
        spec.stages = {
            { Graphics::ShaderStage::Vertex, s_Data.ShaderLibrary.Get("Renderer2D"), "vertexMain" },
            { Graphics::ShaderStage::Fragment, s_Data.ShaderLibrary.Get("Renderer2D"), "fragmentMain" }
        };
        spec.framebufferSpec = framebuffer->GetSpecification();
        spec.blendState = blendState;
        s_Data.QuadShader = Application::Get().GetDevice()->CreateShaderPipeline(layout, spec);

        s_Data.RenderPass->SetCullMode(Graphics::CullMode::None)
            .SetDepthStencilState({.depthCompareFunction = Graphics::DepthCompareFunction::Less, .depthWrite = true});
        glm::mat4 viewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(viewProjection));

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
        BeginScene(camera, Application::Get().GetMainWindowSwapchain()->GetCurrentFrame(), false);
    }

    void Renderer2D::BeginScene(const EditorCamera& camera, Ref<Graphics::Framebuffer> framebuffer, bool clear)
    {
        NB_PROFILE_FUNCTION();

        s_Data.CommandBuffer = s_Data.Device->GetGraphicsQueue()->CreateCommandBuffer();
        s_Data.RenderPass = s_Data.CommandBuffer->CreateRenderPass(framebuffer, clear);

        Graphics::VertexLayout layout = {
            { Graphics::ShaderDataType::Float3, "a_Position" },
            { Graphics::ShaderDataType::Float4, "a_Color" },
            { Graphics::ShaderDataType::Float, "a_TexIndex" },
            { Graphics::ShaderDataType::Float2, "a_TexCoord" },
            { Graphics::ShaderDataType::Float, "a_TilingFactor" },
            { Graphics::ShaderDataType::Int, "a_EntityID" }
        };
        Graphics::BlendState blendState;
        blendState.enabled = true;
        blendState.srcColorFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstColorFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        blendState.srcAlphaFactor = Graphics::BlendFactor::SourceAlpha;
        blendState.dstAlphaFactor = Graphics::BlendFactor::OneMinusSourceAlpha;
        Graphics::PipelineSpecification spec;
        spec.stages = {
            { Graphics::ShaderStage::Vertex, s_Data.ShaderLibrary.Get("Renderer2D"), "vertexMain" },
            { Graphics::ShaderStage::Fragment, s_Data.ShaderLibrary.Get("Renderer2D"), "fragmentMain" }
        };
        spec.framebufferSpec = framebuffer->GetSpecification();
        spec.blendState = blendState;
        s_Data.QuadShader = Application::Get().GetDevice()->CreateShaderPipeline(layout, spec);

        s_Data.RenderPass->SetCullMode(Graphics::CullMode::None)
            .SetDepthStencilState({.depthCompareFunction = Graphics::DepthCompareFunction::Less, .depthWrite = true});
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(camera.GetViewProjectionMatrix()));

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::EndScene()
    {
        NB_PROFILE_FUNCTION();

        auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase));
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        Flush();
    }

    void Renderer2D::NextBash()
    {
        NB_PROFILE_FUNCTION();

        auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase));
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        Ref<Graphics::Framebuffer> framebuffer = s_Data.RenderPass->GetFramebuffer();

        Flush();
        s_Data.CommandBuffer = s_Data.Device->GetGraphicsQueue()->CreateCommandBuffer();
        s_Data.RenderPass = s_Data.CommandBuffer->CreateRenderPass(framebuffer, false);

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::Flush()
    {
        NB_PROFILE_FUNCTION();

        s_Data.RenderPass->SetShaderPipeline(s_Data.QuadShader)
            .SetUniformBlock("Renderer2DUBO", s_Data.QuadUniformBuffer);
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) s_Data.RenderPass->BindTextureToSlot(s_Data.TextureSlots[i], i);
        for (uint32_t i = s_Data.TextureSlotIndex; i < s_Data.MaxTextureSlots; i++) s_Data.RenderPass->BindTextureToSlot(s_Data.WhiteTexture, i);

        s_Data.RenderPass->DrawArrayIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);

        s_Data.RenderPass->EndRenderPass();
        Application::Get().GetDevice()->GetGraphicsQueue()->SubmitCommandBuffer(s_Data.CommandBuffer);

        s_Data.Stats.DrawCalls++;
    }

    void Renderer2D::DrawQuad(const QuadProperties& properties)
    {
        NB_PROFILE_FUNCTION();

        if (s_Data.QuadIndexCount == Renderer2DData::MaxIndices || s_Data.TextureSlotIndex == Renderer2DData::MaxTextureSlots) NextBash();

        glm::mat4 transform = glm::translate(glm::mat4(1.0f), properties.position);
        if (properties.rotation != 0.0f) transform *= glm::rotate(glm::mat4(1.0f), properties.rotation, {0.0f, 0.0f, 1.0f});
        if (properties.size != glm::vec2(1.0f)) transform *= glm::scale(glm::mat4(1.0f), { properties.size.x, properties.size.y, 1.0f });

        float textureIndex = 0.0f;
        if (properties.texture != nullptr) {
            for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
                if (*s_Data.TextureSlots[i] == *properties.texture) {
                    textureIndex = static_cast<float>(i);
                    break;
                }
            }
            if (textureIndex == 0.0f) {
                textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
                s_Data.TextureSlots[s_Data.TextureSlotIndex] = properties.texture;
                s_Data.TextureSlotIndex++;
            }
        }

        s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 0.0f, 0.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 1.0f, 0.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4( 0.5f, 0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 1.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = transform * glm::vec4( -0.5f, 0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    void Renderer2D::DrawQuad(const QuadPropertiesAdvanced& properties)
    {
        NB_PROFILE_FUNCTION();

        if (s_Data.QuadIndexCount == Renderer2DData::MaxIndices || s_Data.TextureSlotIndex == Renderer2DData::MaxTextureSlots) NextBash();

        float textureIndex = 0.0f;
        if (properties.texture != nullptr) {
            for (uint32_t i = 1; i < s_Data.TextureSlotIndex; i++) {
                if (*s_Data.TextureSlots[i] == *properties.texture) {
                    textureIndex = static_cast<float>(i);
                    break;
                }
            }
            if (textureIndex == 0.0f) {
                textureIndex = static_cast<float>(s_Data.TextureSlotIndex);
                s_Data.TextureSlots[s_Data.TextureSlotIndex] = properties.texture;
                s_Data.TextureSlotIndex++;
            }
        }

        s_Data.QuadVertexBufferPtr->Position = properties.transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 0.0f, 0.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = properties.transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 1.0f, 0.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = properties.transform * glm::vec4( 0.5f, 0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 1.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadVertexBufferPtr->Position = properties.transform * glm::vec4( -0.5f, 0.5f, 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->Color = properties.color;
        s_Data.QuadVertexBufferPtr->TexIndex = textureIndex;
        s_Data.QuadVertexBufferPtr->TexCoord = glm::vec2( 0.0f, 1.0f );
        s_Data.QuadVertexBufferPtr->TilingFactor = properties.tilingFactor;
        s_Data.QuadVertexBufferPtr->entityID = properties.entityID;
        s_Data.QuadVertexBufferPtr++;

        s_Data.QuadIndexCount += 6;

        s_Data.Stats.QuadCount++;
    }

    Ref<Graphics::Texture2D> Renderer2D::GetWhiteTexture()
    {
        return s_Data.WhiteTexture;
    }

    Renderer2D::Statistics Renderer2D::GetStatistics()
    {
        return s_Data.Stats;
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }
}
