#include "Renderer2D.h"

#include <glm/gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "Shader.h"
#include "Buffers/VertexArray.h"
#include "Cameras/Camera.h"
#include "Core/RenderCommand.h"
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
        static constexpr uint32_t MaxTextureSlots = 16;

        Ref<VertexArray> QuadVertexArray;
        Ref<VertexBuffer> QuadVertexBuffer;
        Ref<Shader> QuadShader;
        Ref<UniformBuffer> QuadUniformBuffer;
        Ref<Texture2D> WhiteTexture;

        uint32_t QuadIndexCount = 0;
        QuadVertex* QuadVertexBufferBase = nullptr;
        QuadVertex* QuadVertexBufferPtr = nullptr;

        std::array<Ref<Texture2D>, MaxTextureSlots> TextureSlots;
        uint32_t TextureSlotIndex = 1;

        Renderer2D::Statistics Stats;

        bool CullingEnabled = false;
    };

    static Renderer2DData s_Data;

    void Renderer2D::Init()
    {
        NB_PROFILE_FUNCTION();

        s_Data.QuadVertexArray = VertexArray::Create();

        s_Data.QuadVertexBuffer = VertexBuffer::Create(Renderer2DData::MaxVertices * sizeof(QuadVertex));
        s_Data.QuadVertexBuffer->SetLayout({
            {ShaderDataType::Float3, "a_Position"},
            {ShaderDataType::Float4, "a_Color"},
            {ShaderDataType::Float, "a_TexIndex"},
            {ShaderDataType::Float2, "a_TexCoord"},
            {ShaderDataType::Float, "a_TilingFactor"},
            {ShaderDataType::Int, "a_EntityID"}
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

        Ref<IndexBuffer> indexBuffer = IndexBuffer::Create(quadIndices, Nebula::Renderer2DData::MaxIndices);
        s_Data.QuadVertexArray->SetIndexBuffer(indexBuffer);
        delete[] quadIndices;

        int32_t samplers[Renderer2DData::MaxTextureSlots];
        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
            samplers[i] = i;

        s_Data.QuadShader = Shader::Create(Platform::Files::GetResourcesPath() / "assets/NebulaEngine/shaders/Renderer2D.slang");

        s_Data.QuadShader->Bind();
        s_Data.QuadShader->SetIntArray("u_Textures", samplers, Renderer2DData::MaxTextureSlots);
        s_Data.QuadShader->Unbind();

        s_Data.QuadUniformBuffer = UniformBuffer::Create({
            {ShaderDataType::Mat4, "u_ViewProjection"},
            {ShaderDataType::Float, "u_TilingFactor"}
        });

        s_Data.WhiteTexture = Texture2D::Create(1, 1, Texture::InternalFormat::RGBA8);
        uint32_t whiteTextureData = 0xffffffff;
        s_Data.WhiteTexture->SetData(&whiteTextureData, sizeof(uint32_t), Texture::DataFormat::RGBA);

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

        delete s_Data.QuadVertexBufferBase;

        for (uint32_t i = 0; i < Renderer2DData::MaxTextureSlots; i++)
            s_Data.TextureSlots[i].reset();
    }

    void Renderer2D::BeginScene(const OrthographicCamera& camera)
    {
        NB_PROFILE_FUNCTION();

        SetupRenderState();

        s_Data.CullingEnabled = RenderCommand::IsFaceCullingEnabled();
        RenderCommand::EnableFaceCulling(false);

        s_Data.QuadShader->Bind();
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(camera.getViewProjectionMatrix()));
        s_Data.QuadShader->SetUniformBlock("Renderer2DUBO", s_Data.QuadUniformBuffer);

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::BeginScene(const Camera& camera, const glm::mat4& transform)
    {
        NB_PROFILE_FUNCTION();

        SetupRenderState();

        s_Data.CullingEnabled = RenderCommand::IsFaceCullingEnabled();
        RenderCommand::EnableFaceCulling(false);

        s_Data.QuadShader->Bind();
        glm::mat4 viewProjection = camera.GetProjectionMatrix() * glm::inverse(transform);
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(viewProjection));
        s_Data.QuadShader->SetUniformBlock("Renderer2DUBO", s_Data.QuadUniformBuffer);

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::BeginScene(const EditorCamera& camera)
    {
        NB_PROFILE_FUNCTION();

        SetupRenderState();

        s_Data.CullingEnabled = RenderCommand::IsFaceCullingEnabled();
        RenderCommand::EnableFaceCulling(false);

        s_Data.QuadShader->Bind();
        s_Data.QuadUniformBuffer->SetData(0, glm::value_ptr(camera.GetViewProjectionMatrix()));
        s_Data.QuadShader->SetUniformBlock("Renderer2DUBO", s_Data.QuadUniformBuffer);

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

        RenderCommand::EnableFaceCulling(s_Data.CullingEnabled);
    }

    void Renderer2D::NextBash()
    {
        NB_PROFILE_FUNCTION();

        auto dataSize = static_cast<uint32_t>(reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferPtr) - reinterpret_cast<uint8_t*>(s_Data.QuadVertexBufferBase));
        s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBufferBase, dataSize);

        Flush();

        s_Data.QuadIndexCount = 0;
        s_Data.QuadVertexBufferPtr = s_Data.QuadVertexBufferBase;

        s_Data.TextureSlotIndex = 1;
    }

    void Renderer2D::Flush()
    {
        NB_PROFILE_FUNCTION();

        s_Data.QuadShader->Bind();
        for (uint32_t i = 0; i < s_Data.TextureSlotIndex; i++) s_Data.TextureSlots[i]->Bind(i);

        s_Data.QuadVertexArray->Bind();
        RenderCommand::DrawIndexed(s_Data.QuadVertexArray, s_Data.QuadIndexCount);
        s_Data.QuadVertexArray->Unbind();
        s_Data.QuadShader->Unbind();

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

    Renderer2D::Statistics Renderer2D::GetStatistics()
    {
        return s_Data.Stats;
    }

    void Renderer2D::ResetStats()
    {
        memset(&s_Data.Stats, 0, sizeof(Statistics));
    }

    void Renderer2D::SetupRenderState()
    {
        RendererAPI::BlendState state;
        state.enabled = true;
        state.srcColorFactor = RendererAPI::BlendFactor::SourceAlpha;
        state.dstColorFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
        state.srcAlphaFactor = RendererAPI::BlendFactor::SourceAlpha;
        state.dstAlphaFactor = RendererAPI::BlendFactor::OneMinusSourceAlpha;
        RenderCommand::SetBlendingState(state);
        RenderCommand::EnableFaceCulling(false);
        RenderCommand::EnableDepthTest(true);
        RenderCommand::EnableScissorTest(false);
    }
}
