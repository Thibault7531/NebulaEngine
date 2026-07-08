#pragma once

#include "Cameras/Camera.h"
#include "Cameras/EditorCamera.h"
#include "Nebula/CoreSystems/Core/Core.h"
#include "Cameras/OrthographicCamera.h"
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
            int32_t entityID = -1;
        };
        struct QuadPropertiesAdvanced
        {
            glm::mat4 transform = glm::mat4(1.0f);
            Ref<Texture2D> texture = nullptr;
            glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f };
            float tilingFactor = 1.0f;
            int32_t entityID = -1;
        };

    public:
        static void Init();
        static void Shutdown();

        static void BeginScene(const OrthographicCamera& camera);
        static void BeginScene(const Camera& camera, const glm::mat4& transform);
        static void BeginScene(const EditorCamera& camera);
        static void EndScene();
        static void NextBash();
        static void Flush();

        static void DrawQuad(const QuadProperties& properties);
        static void DrawQuad(const QuadPropertiesAdvanced& properties);

        struct Statistics
        {
            uint32_t DrawCalls = 0;
            uint32_t QuadCount = 0;

            uint32_t GetTotalVertexCount() const { return QuadCount * 4; }
            uint32_t GetTotalIndexCount() const { return QuadCount * 6; }
        };
        static Statistics GetStatistics();
        static void ResetStats();

    private:
        static void SetupRenderState();

    };

}
