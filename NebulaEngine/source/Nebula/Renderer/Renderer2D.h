#pragma once

#include "Camera.h"
#include "Texture.h"

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

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void EndScene();

        [[deprecated]] static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawQuad(const glm::vec2& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);
        [[deprecated]] static void DrawQuad(const glm::vec3& position, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);

        [[deprecated]] static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const glm::vec4& color);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec2& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);
        [[deprecated]] static void DrawRotatedQuad(const glm::vec3& position, const float& rotation, const glm::vec2& size, const Ref<Texture2D>& texture, const glm::vec4& color = {1.0f, 1.0f, 1.0f, 1.0f}, float tilingFactor = 1.0f);

        static void DrawQuad(const QuadProperties& properties);

    };

}
