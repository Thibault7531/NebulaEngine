#pragma once

#include <imgui.h>
#include <glad/gl.h>

#include "Core/Nebula/ImGui/ImGuiRenderer.h"

namespace Nebula::OpenGL410
{
    struct RendererData
    {
        GLuint GlVersion;
        char GlslVersionString[32];

        bool GlProfileIsCompat;
        bool UseBufferSubData;
        bool HasPolygonMode;
        bool HasClipOrigin;

        GLint GlProfileMask;

        unsigned int VboHandle;
        unsigned int ElementsHandle;
        GLuint ShaderHandle;
        GLuint FontTexture;

        GLint AttribLocationTex;
        GLint AttribLocationProjMtx;
        GLuint AttribLocationVtxPos;
        GLuint AttribLocationVtxUV;
        GLuint AttribLocationVtxColor;

        GLsizeiptr VertexBufferSize;
        GLsizeiptr IndexBufferSize;

        RendererData() { memset((void*)this, 0, sizeof(*this)); }
    };

    class OpenGL410ImGuiRenderer : public ImGuiRenderer
    {
    public:
        OpenGL410ImGuiRenderer()
            : io(ImGui::GetIO()), platform_io(ImGui::GetPlatformIO()) {}
        virtual ~OpenGL410ImGuiRenderer() {}

        virtual void Init() override;
        virtual void Shutdown() override;

        virtual void NewFrame() override;

        virtual void Render(ImDrawData* draw_data) override;

    private:
        void InitMultipleViewports();
        void ShutdownMultipleViewports();

        void CreateDeviceObjects();
        void CreateFontsTexture();
        void DestroyDeviceObjects();
        void DestroyFontsTexture();

        bool CheckShader(GLuint handle, const char* desc);
        bool CheckProgram(GLuint handle, const char* desc);

    public:
        static void RenderStatic(ImDrawData* draw_data);
        static void SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object);
        static void RenderWindow(ImGuiViewport* viewport, void*);

    private:
        ImGuiIO& io;
        ImGuiPlatformIO& platform_io;

        RendererData* renderer_data;
    };

}
