#pragma once

#include "Camera.h"
#include "Nebula/Renderer/OrthographicCamera.h"
#include "Nebula/Renderer/Texture.h"

namespace Nebula
{

    class Renderer2D
    {
    public:
        struct QuadProperties
        {
            glm::vec3 position = glm::vec3(0.0f);
            float rotation = 0.0f;
            glm::vec2 size = { 1.0f, 1.0f };
            Ref<Texture2D> texture = nullptr;
            glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
            float tilingFactor = 1.0f;
        };
        struct QuadPropertiesAdvanced
        {
            glm::mat4 transform = glm::mat4(1.0f);
            Ref<Texture2D> texture = nullptr;
            glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
            float tilingFactor = 1.0f;
        };

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void EndScene();
        static void NextBash();
        static void Flush();

        [[deprecated]] static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);
        [[deprecated]] static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);

        [[deprecated]] static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);

        static void DrawQuad(const QuadProperties& properties);
        static void DrawQuad(const QuadPropertiesAdvanced& properties);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() { return QuadCount * 6; }
        };
        static Statistics GetStatistics();
        static void ResetStats();

    };

}
