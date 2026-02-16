#include "OpenGLImGuiRenderer.h"

namespace Nebula
{

    void OpenGLImGuiRenderer::Init()
    {
        NB_CORE_DEBUG("Initializing Nebula::Renderer::OpenGL for ImGui");

        // Setup backend capabilities flags
        renderer_data = new RendererData();
        io.BackendRendererUserData = static_cast<void*>(renderer_data);
        io.BackendRendererName = "Nebula::Renderer::OpenGL";

        // Query for GL version (e.g. 320 for GL 3.2)
        // Desktop or GLES 3
        const char* gl_version_str = reinterpret_cast<const char*>(glGetString(GL_VERSION));
        GLint major = 0;
        GLint minor = 0;
        glGetIntegerv(GL_MAJOR_VERSION, &major);
        glGetIntegerv(GL_MINOR_VERSION, &minor);
        if (major == 0 && minor == 0)
            sscanf(gl_version_str, "%d.%d", &major, &minor);
        renderer_data->GlVersion = static_cast<GLuint>(major * 100 + minor * 10);
    #if defined(GL_CONTEXT_PROFILE_MASK)
        if (renderer_data->GlVersion >= 320)
            glGetIntegerv(GL_CONTEXT_PROFILE_MASK, &renderer_data->GlProfileMask);
        renderer_data->GlProfileIsCompat = (renderer_data->GlProfileMask & GL_CONTEXT_COMPATIBILITY_PROFILE_BIT) != 0;
    #endif

        renderer_data->UseBufferSubData = false;

        io.BackendFlags |= ImGuiBackendFlags_RendererHasViewports;  // We can create multi-viewports on the Renderer side (optional)

        // Store GLSL version string so we can refer to it later in case we recreate shaders.
        // Note: GLSL version is NOT the same as GL version. Leave this to nullptr if unsure.
        strcpy(renderer_data->GlslVersionString, "#version 460");
        strcat(renderer_data->GlslVersionString, "\n");

        // Make an arbitrary GL call (we don't actually need the result)
        // IF YOU GET A CRASH HERE: it probably means the OpenGL function loader didn't do its job. Let us know!
        GLint current_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &current_texture);

        // Detect extensions we support
        renderer_data->HasPolygonMode = true;
        renderer_data->HasClipOrigin = (renderer_data->GlVersion >= 450);
        GLint num_extensions = 0;
        glGetIntegerv(GL_NUM_EXTENSIONS, &num_extensions);
        for (GLint i = 0; i < num_extensions; i++)
        {
            const char* extension = (const char*)glGetStringi(GL_EXTENSIONS, i);
            if (extension != nullptr && strcmp(extension, "GL_ARB_clip_control") == 0)
                renderer_data->HasClipOrigin = true;
        }

        InitMultipleViewports();
    }

    void OpenGLImGuiRenderer::Shutdown()
    {
        NB_CORE_DEBUG("Shutting down {0}", io.BackendRendererName);

        ShutdownMultipleViewports();

        DestroyDeviceObjects();
        io.BackendRendererName = nullptr;
        io.BackendRendererUserData = nullptr;
        io.BackendFlags &= ~(ImGuiBackendFlags_RendererHasVtxOffset | ImGuiBackendFlags_RendererHasViewports);
        delete renderer_data;
    }

    void OpenGLImGuiRenderer::NewFrame()
    {
        if (!renderer_data->ShaderHandle)
            CreateDeviceObjects();
        if (!renderer_data->FontTexture)
            CreateFontsTexture();
    }

    void OpenGLImGuiRenderer::Render(ImDrawData* draw_data)
    {
        RenderStatic(draw_data);
    }

    void OpenGLImGuiRenderer::InitMultipleViewports()
    {
        platform_io.Renderer_RenderWindow = RenderWindow;
    }

    void OpenGLImGuiRenderer::ShutdownMultipleViewports()
    {
        ImGui::DestroyPlatformWindows();
    }

    void OpenGLImGuiRenderer::CreateDeviceObjects()
    {
        // Backup GL state
        GLint last_texture, last_array_buffer;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGetIntegerv(GL_ARRAY_BUFFER_BINDING, &last_array_buffer);
        GLint last_pixel_unpack_buffer = 0;
        if (renderer_data->GlVersion >= 210) { glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING, &last_pixel_unpack_buffer); glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0); }
        GLint last_vertex_array;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &last_vertex_array);

        // Parse GLSL version string
        int glsl_version = 130;
        sscanf(renderer_data->GlslVersionString, "#version %d", &glsl_version);

        const GLchar* vertex_shader_glsl_410_core =
            "layout (location = 0) in vec2 Position;\n"
            "layout (location = 1) in vec2 UV;\n"
            "layout (location = 2) in vec4 Color;\n"
            "uniform mat4 ProjMtx;\n"
            "out vec2 Frag_UV;\n"
            "out vec4 Frag_Color;\n"
            "void main()\n"
            "{\n"
            "    Frag_UV = UV;\n"
            "    Frag_Color = Color;\n"
            "    gl_Position = ProjMtx * vec4(Position.xy,0,1);\n"
            "}\n";

        const GLchar* fragment_shader_glsl_410_core =
            "in vec2 Frag_UV;\n"
            "in vec4 Frag_Color;\n"
            "uniform sampler2D Texture;\n"
            "layout (location = 0) out vec4 Out_Color;\n"
            "void main()\n"
            "{\n"
            "    Out_Color = Frag_Color * texture(Texture, Frag_UV.st);\n"
            "}\n";

        // Select shaders matching our GLSL versions
        const GLchar* vertex_shader = vertex_shader_glsl_410_core;
        const GLchar* fragment_shader = fragment_shader_glsl_410_core;

        // Create shaders
        const GLchar* vertex_shader_with_version[2] = { renderer_data->GlslVersionString, vertex_shader };
        GLuint vert_handle;
        vert_handle = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vert_handle, 2, vertex_shader_with_version, nullptr);
        glCompileShader(vert_handle);
        CheckShader(vert_handle, "vertex shader");

        const GLchar* fragment_shader_with_version[2] = { renderer_data->GlslVersionString, fragment_shader };
        GLuint frag_handle;
        frag_handle = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(frag_handle, 2, fragment_shader_with_version, nullptr);
        glCompileShader(frag_handle);
        CheckShader(frag_handle, "fragment shader");

        // Link
        renderer_data->ShaderHandle = glCreateProgram();
        glAttachShader(renderer_data->ShaderHandle, vert_handle);
        glAttachShader(renderer_data->ShaderHandle, frag_handle);
        glLinkProgram(renderer_data->ShaderHandle);
        CheckProgram(renderer_data->ShaderHandle, "shader program");

        glDetachShader(renderer_data->ShaderHandle, vert_handle);
        glDetachShader(renderer_data->ShaderHandle, frag_handle);
        glDeleteShader(vert_handle);
        glDeleteShader(frag_handle);

        renderer_data->AttribLocationTex = glGetUniformLocation(renderer_data->ShaderHandle, "Texture");
        renderer_data->AttribLocationProjMtx = glGetUniformLocation(renderer_data->ShaderHandle, "ProjMtx");
        renderer_data->AttribLocationVtxPos = static_cast<GLuint>(glGetAttribLocation(renderer_data->ShaderHandle, "Position"));
        renderer_data->AttribLocationVtxUV = static_cast<GLuint>(glGetAttribLocation(renderer_data->ShaderHandle, "UV"));
        renderer_data->AttribLocationVtxColor = static_cast<GLuint>(glGetAttribLocation(renderer_data->ShaderHandle, "Color"));

        // Create buffers
        glGenBuffers(1, &renderer_data->VboHandle);
        glGenBuffers(1, &renderer_data->ElementsHandle);

        CreateFontsTexture();

        // Restore modified GL state
        glBindTexture(GL_TEXTURE_2D, last_texture);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        if (renderer_data->GlVersion >= 210) { glBindBuffer(GL_PIXEL_UNPACK_BUFFER, last_pixel_unpack_buffer); }
        glBindVertexArray(last_vertex_array);
    }

    void OpenGLImGuiRenderer::CreateFontsTexture()
    {
        // Build texture atlas
        unsigned char* pixels;
        int width, height;
        io.Fonts->GetTexDataAsRGBA32(&pixels, &width, &height);   // Load as RGBA 32-bit (75% of the memory is wasted, but default font is so small) because it is more likely to be compatible with user's existing shaders. If your ImTextureId represent a higher-level concept than just a GL texture id, consider calling GetTexDataAsAlpha8() instead to save on GPU memory.

        // Upload texture to graphics system
        // (Bilinear sampling is required by default. Set 'io.Fonts->Flags |= ImFontAtlasFlags_NoBakedLines' or 'style.AntiAliasedLinesUseTex = false' to allow point/nearest sampling)
        GLint last_texture;
        glGetIntegerv(GL_TEXTURE_BINDING_2D, &last_texture);
        glGenTextures(1, &renderer_data->FontTexture);
        glBindTexture(GL_TEXTURE_2D, renderer_data->FontTexture);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
#ifdef GL_UNPACK_ROW_LENGTH // Not on WebGL/ES
        glPixelStorei(GL_UNPACK_ROW_LENGTH, 0);
#endif
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);

        // Store identifier
        io.Fonts->SetTexID(static_cast<ImTextureID>(static_cast<intptr_t>(renderer_data->FontTexture)));

        // Restore state
        glBindTexture(GL_TEXTURE_2D, last_texture);
    }

    void OpenGLImGuiRenderer::DestroyDeviceObjects()
    {
        if (renderer_data->VboHandle)      { glDeleteBuffers(1, &renderer_data->VboHandle); renderer_data->VboHandle = 0; }
        if (renderer_data->ElementsHandle) { glDeleteBuffers(1, &renderer_data->ElementsHandle); renderer_data->ElementsHandle = 0; }
        if (renderer_data->ShaderHandle)   { glDeleteProgram(renderer_data->ShaderHandle); renderer_data->ShaderHandle = 0; }
        DestroyFontsTexture();
    }

    void OpenGLImGuiRenderer::DestroyFontsTexture()
    {
        if (renderer_data->FontTexture)
        {
            glDeleteTextures(1, &renderer_data->FontTexture);
            io.Fonts->SetTexID(0);
            renderer_data->FontTexture = 0;
        }
    }

    bool OpenGLImGuiRenderer::CheckShader(GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
        glGetShaderiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (static_cast<GLboolean>(status) == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to compile %s! With GLSL: %s\n", desc, renderer_data->GlslVersionString);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetShaderInfoLog(handle, log_length, nullptr, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return static_cast<GLboolean>(status) == GL_TRUE;
    }

    bool OpenGLImGuiRenderer::CheckProgram(GLuint handle, const char* desc)
    {
        GLint status = 0, log_length = 0;
        glGetProgramiv(handle, GL_LINK_STATUS, &status);
        glGetProgramiv(handle, GL_INFO_LOG_LENGTH, &log_length);
        if (static_cast<GLboolean>(status) == GL_FALSE)
            fprintf(stderr, "ERROR: ImGui_ImplOpenGL3_CreateDeviceObjects: failed to link %s! With GLSL %s\n", desc, renderer_data->GlslVersionString);
        if (log_length > 1)
        {
            ImVector<char> buf;
            buf.resize((int)(log_length + 1));
            glGetProgramInfoLog(handle, log_length, nullptr, (GLchar*)buf.begin());
            fprintf(stderr, "%s\n", buf.begin());
        }
        return static_cast<GLboolean>(status) == GL_TRUE;
    }

    void OpenGLImGuiRenderer::RenderStatic(ImDrawData* draw_data)
    {
        auto renderer_data = static_cast<RendererData*>(ImGui::GetIO().BackendRendererUserData);

        // Avoid rendering when minimized, scale coordinates for retina displays (screen coordinates != framebuffer coordinates)
        int fb_width = static_cast<int>(draw_data->DisplaySize.x * draw_data->FramebufferScale.x);
        int fb_height = static_cast<int>(draw_data->DisplaySize.y * draw_data->FramebufferScale.y);
        if (fb_width <= 0 || fb_height <= 0)
            return;

        // Backup GL state
        GLenum last_active_texture; glGetIntegerv(GL_ACTIVE_TEXTURE, reinterpret_cast<GLint*>(&last_active_texture));
        glActiveTexture(GL_TEXTURE0);
        GLuint last_program; glGetIntegerv(GL_CURRENT_PROGRAM, reinterpret_cast<GLint*>(&last_program));
        GLuint last_texture; glGetIntegerv(GL_TEXTURE_BINDING_2D, reinterpret_cast<GLint*>(&last_texture));
        GLuint last_sampler; if (renderer_data->GlVersion >= 330) { glGetIntegerv(GL_SAMPLER_BINDING, reinterpret_cast<GLint*>(&last_sampler)); } else { last_sampler = 0; }
        GLuint last_array_buffer; glGetIntegerv(GL_ARRAY_BUFFER_BINDING, reinterpret_cast<GLint*>(&last_array_buffer));
        GLuint last_vertex_array_object; glGetIntegerv(GL_VERTEX_ARRAY_BINDING, reinterpret_cast<GLint*>(&last_vertex_array_object));
        GLint last_polygon_mode[2]; if (renderer_data->HasPolygonMode) { glGetIntegerv(GL_POLYGON_MODE, last_polygon_mode); }
        GLint last_viewport[4]; glGetIntegerv(GL_VIEWPORT, last_viewport);
        GLint last_scissor_box[4]; glGetIntegerv(GL_SCISSOR_BOX, last_scissor_box);
        GLenum last_blend_src_rgb; glGetIntegerv(GL_BLEND_SRC_RGB, reinterpret_cast<GLint*>(&last_blend_src_rgb));
        GLenum last_blend_dst_rgb; glGetIntegerv(GL_BLEND_DST_RGB, reinterpret_cast<GLint*>(&last_blend_dst_rgb));
        GLenum last_blend_src_alpha; glGetIntegerv(GL_BLEND_SRC_ALPHA, reinterpret_cast<GLint*>(&last_blend_src_alpha));
        GLenum last_blend_dst_alpha; glGetIntegerv(GL_BLEND_DST_ALPHA, reinterpret_cast<GLint*>(&last_blend_dst_alpha));
        GLenum last_blend_equation_rgb; glGetIntegerv(GL_BLEND_EQUATION_RGB, reinterpret_cast<GLint*>(&last_blend_equation_rgb));
        GLenum last_blend_equation_alpha; glGetIntegerv(GL_BLEND_EQUATION_ALPHA, reinterpret_cast<GLint*>(&last_blend_equation_alpha));
        GLboolean last_enable_blend = glIsEnabled(GL_BLEND);
        GLboolean last_enable_cull_face = glIsEnabled(GL_CULL_FACE);
        GLboolean last_enable_depth_test = glIsEnabled(GL_DEPTH_TEST);
        GLboolean last_enable_stencil_test = glIsEnabled(GL_STENCIL_TEST);
        GLboolean last_enable_scissor_test = glIsEnabled(GL_SCISSOR_TEST);
        GLboolean last_enable_primitive_restart = (renderer_data->GlVersion >= 310) ? glIsEnabled(GL_PRIMITIVE_RESTART) : GL_FALSE;

        // Setup desired GL state
        // Recreate the VAO every time (this is to easily allow multiple GL contexts to be rendered to. VAO are not shared among GL contexts)
        // The renderer would actually work without any VAO bound, but then our VertexAttrib calls would overwrite the default one currently bound.
        GLuint vertex_array_object = 0;
        glGenVertexArrays(1, &vertex_array_object);
        SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);

        // Will project scissor/clipping rectangles into framebuffer space
        ImVec2 clip_off = draw_data->DisplayPos;         // (0,0) unless using multi-viewports
        ImVec2 clip_scale = draw_data->FramebufferScale; // (1,1) unless using retina display which are often (2,2)

        // Render command lists
        for (int n = 0; n < draw_data->CmdListsCount; n++)
        {
            const ImDrawList* draw_list = draw_data->CmdLists[n];

            // Upload vertex/index buffers
            // - OpenGL drivers are in a very sorry state nowadays....
            //   During 2021 we attempted to switch from glBufferData() to orphaning+glBufferSubData() following reports
            //   of leaks on Intel GPU when using multi-viewports on Windows.
            // - After this we kept hearing of various display corruptions issues. We started disabling on non-Intel GPU, but issues still got reported on Intel.
            // - We are now back to using exclusively glBufferData(). So bd->UseBufferSubData IS ALWAYS FALSE in this code.
            //   We are keeping the old code path for a while in case people finding new issues may want to test the bd->UseBufferSubData path.
            // - See https://github.com/ocornut/imgui/issues/4468 and please report any corruption issues.
            const GLsizeiptr vtx_buffer_size = static_cast<GLsizeiptr>(draw_list->VtxBuffer.Size) * static_cast<int>(sizeof(ImDrawVert));
            const GLsizeiptr idx_buffer_size = static_cast<GLsizeiptr>(draw_list->IdxBuffer.Size) * static_cast<int>(sizeof(ImDrawIdx));
            if (renderer_data->UseBufferSubData)
            {
                if (renderer_data->VertexBufferSize < vtx_buffer_size)
                {
                    renderer_data->VertexBufferSize = vtx_buffer_size;
                    glBufferData(GL_ARRAY_BUFFER, renderer_data->VertexBufferSize, nullptr, GL_STREAM_DRAW);
                }
                if (renderer_data->IndexBufferSize < idx_buffer_size)
                {
                    renderer_data->IndexBufferSize = idx_buffer_size;
                    glBufferData(GL_ELEMENT_ARRAY_BUFFER, renderer_data->IndexBufferSize, nullptr, GL_STREAM_DRAW);
                }
                glBufferSubData(GL_ARRAY_BUFFER, 0, vtx_buffer_size, static_cast<const GLvoid*>(draw_list->VtxBuffer.Data));
                glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, idx_buffer_size, static_cast<const GLvoid*>(draw_list->IdxBuffer.Data));
            }
            else
            {
                glBufferData(GL_ARRAY_BUFFER, vtx_buffer_size, static_cast<const GLvoid*>(draw_list->VtxBuffer.Data), GL_STREAM_DRAW);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx_buffer_size, static_cast<const GLvoid*>(draw_list->IdxBuffer.Data), GL_STREAM_DRAW);
            }

            for (int cmd_i = 0; cmd_i < draw_list->CmdBuffer.Size; cmd_i++)
            {
                const ImDrawCmd* pcmd = &draw_list->CmdBuffer[cmd_i];
                if (pcmd->UserCallback != nullptr)
                {
                    // User callback, registered via ImDrawList::AddCallback()
                    // (ImDrawCallback_ResetRenderState is a special callback value used by the user to request the renderer to reset render state.)
                    if (pcmd->UserCallback == ImDrawCallback_ResetRenderState)
                        SetupRenderState(draw_data, fb_width, fb_height, vertex_array_object);
                    else
                        pcmd->UserCallback(draw_list, pcmd);
                }
                else
                {
                    // Project scissor/clipping rectangles into framebuffer space
                    ImVec2 clip_min((pcmd->ClipRect.x - clip_off.x) * clip_scale.x, (pcmd->ClipRect.y - clip_off.y) * clip_scale.y);
                    ImVec2 clip_max((pcmd->ClipRect.z - clip_off.x) * clip_scale.x, (pcmd->ClipRect.w - clip_off.y) * clip_scale.y);
                    if (clip_max.x <= clip_min.x || clip_max.y <= clip_min.y)
                        continue;

                    // Apply scissor/clipping rectangle (Y is inverted in OpenGL)
                    glScissor(static_cast<int>(clip_min.x), static_cast<int>(static_cast<float>(fb_height) - clip_max.y), static_cast<int>(clip_max.x - clip_min.x), static_cast<int>(clip_max.y - clip_min.y));

                    // Bind texture, Draw
                    glBindTexture(GL_TEXTURE_2D, static_cast<GLuint>(static_cast<intptr_t>(pcmd->GetTexID())));
                    if (renderer_data->GlVersion >= 320)
                        glDrawElementsBaseVertex(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, reinterpret_cast<void*>(static_cast<intptr_t>(pcmd->IdxOffset * sizeof(ImDrawIdx))), static_cast<GLint>(pcmd->VtxOffset));
                    else
                        glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(pcmd->ElemCount), sizeof(ImDrawIdx) == 2 ? GL_UNSIGNED_SHORT : GL_UNSIGNED_INT, reinterpret_cast<void*>(static_cast<intptr_t>(pcmd->IdxOffset * sizeof(ImDrawIdx))));
                }
            }
        }

        // Destroy the temporary VAO
        glDeleteVertexArrays(1, &vertex_array_object);

        // Restore modified GL state
        // This "glIsProgram()" check is required because if the program is "pending deletion" at the time of binding backup, it will have been deleted by now and will cause an OpenGL error. See #6220.
        if (last_program == 0 || glIsProgram(last_program)) glUseProgram(last_program);
        glBindTexture(GL_TEXTURE_2D, last_texture);
        if (renderer_data->GlVersion >= 330)
            glBindSampler(0, last_sampler);
        glActiveTexture(last_active_texture);
        glBindVertexArray(last_vertex_array_object);
        glBindBuffer(GL_ARRAY_BUFFER, last_array_buffer);
        glBlendEquationSeparate(last_blend_equation_rgb, last_blend_equation_alpha);
        glBlendFuncSeparate(last_blend_src_rgb, last_blend_dst_rgb, last_blend_src_alpha, last_blend_dst_alpha);
        if (last_enable_blend) glEnable(GL_BLEND); else glDisable(GL_BLEND);
        if (last_enable_cull_face) glEnable(GL_CULL_FACE); else glDisable(GL_CULL_FACE);
        if (last_enable_depth_test) glEnable(GL_DEPTH_TEST); else glDisable(GL_DEPTH_TEST);
        if (last_enable_stencil_test) glEnable(GL_STENCIL_TEST); else glDisable(GL_STENCIL_TEST);
        if (last_enable_scissor_test) glEnable(GL_SCISSOR_TEST); else glDisable(GL_SCISSOR_TEST);
        if (renderer_data->GlVersion >= 310) { if (last_enable_primitive_restart) glEnable(GL_PRIMITIVE_RESTART); else glDisable(GL_PRIMITIVE_RESTART); }

        // Desktop OpenGL 3.0 and OpenGL 3.1 had separate polygon draw modes for front-facing and back-facing faces of polygons
        if (renderer_data->HasPolygonMode) { if (renderer_data->GlVersion <= 310) { glPolygonMode(GL_FRONT, static_cast<GLenum>(last_polygon_mode[0])); glPolygonMode(GL_BACK, static_cast<GLenum>(last_polygon_mode[1])); } else { glPolygonMode(GL_FRONT_AND_BACK, static_cast<GLenum>(last_polygon_mode[0])); } }

        glViewport(last_viewport[0], last_viewport[1], (GLsizei)last_viewport[2], (GLsizei)last_viewport[3]);
        glScissor(last_scissor_box[0], last_scissor_box[1], (GLsizei)last_scissor_box[2], (GLsizei)last_scissor_box[3]);
    }

    void OpenGLImGuiRenderer::SetupRenderState(ImDrawData* draw_data, int fb_width, int fb_height, GLuint vertex_array_object)
    {
        auto* renderer_data = static_cast<RendererData*>(ImGui::GetIO().BackendRendererUserData);

        // Setup render state: alpha-blending enabled, no face culling, no depth testing, scissor enabled, polygon fill
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFuncSeparate(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA, GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
        glDisable(GL_CULL_FACE);
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_STENCIL_TEST);
        glEnable(GL_SCISSOR_TEST);
        if (renderer_data->GlVersion >= 310)
            glDisable(GL_PRIMITIVE_RESTART);
        if (renderer_data->HasPolygonMode)
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        // Support for GL 4.5 rarely used glClipControl(GL_UPPER_LEFT)
#if defined(GL_CLIP_ORIGIN)
        bool clip_origin_lower_left = true;
        if (renderer_data->HasClipOrigin)
        {
            GLenum current_clip_origin = 0; glGetIntegerv(GL_CLIP_ORIGIN, reinterpret_cast<GLint*>(&current_clip_origin));
            if (current_clip_origin == GL_UPPER_LEFT)
                clip_origin_lower_left = false;
        }
#endif

        // Setup viewport, orthographic projection matrix
        // Our visible imgui space lies from draw_data->DisplayPos (top left) to draw_data->DisplayPos+data_data->DisplaySize (bottom right). DisplayPos is (0,0) for single viewport apps.
        glViewport(0, 0, (GLsizei)fb_width, (GLsizei)fb_height);
        float L = draw_data->DisplayPos.x;
        float R = draw_data->DisplayPos.x + draw_data->DisplaySize.x;
        float T = draw_data->DisplayPos.y;
        float B = draw_data->DisplayPos.y + draw_data->DisplaySize.y;
    #if defined(GL_CLIP_ORIGIN)
        if (!clip_origin_lower_left) { float tmp = T; T = B; B = tmp; } // Swap top and bottom if origin is upper left
    #endif
        const float ortho_projection[4][4] =
        {
            { 2.0f/(R-L),   0.0f,         0.0f,   0.0f },
            { 0.0f,         2.0f/(T-B),   0.0f,   0.0f },
            { 0.0f,         0.0f,        -1.0f,   0.0f },
            { (R+L)/(L-R),  (T+B)/(B-T),  0.0f,   1.0f },
        };
        glUseProgram(renderer_data->ShaderHandle);
        glUniform1i(renderer_data->AttribLocationTex, 0);
        glUniformMatrix4fv(renderer_data->AttribLocationProjMtx, 1, GL_FALSE, &ortho_projection[0][0]);

        if (renderer_data->GlVersion >= 330)
            glBindSampler(0, 0); // We use combined texture/sampler state. Applications using GL 3.3 and GL ES 3.0 may set that otherwise.

        (void)vertex_array_object;
        glBindVertexArray(vertex_array_object);

        // Bind vertex/index buffers and setup attributes for ImDrawVert
        glBindBuffer(GL_ARRAY_BUFFER, renderer_data->VboHandle);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, renderer_data->ElementsHandle);
        glEnableVertexAttribArray(renderer_data->AttribLocationVtxPos);
        glEnableVertexAttribArray(renderer_data->AttribLocationVtxUV);
        glEnableVertexAttribArray(renderer_data->AttribLocationVtxColor);
        glVertexAttribPointer(renderer_data->AttribLocationVtxPos,   2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, pos)));
        glVertexAttribPointer(renderer_data->AttribLocationVtxUV,    2, GL_FLOAT,         GL_FALSE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, uv)));
        glVertexAttribPointer(renderer_data->AttribLocationVtxColor, 4, GL_UNSIGNED_BYTE, GL_TRUE, sizeof(ImDrawVert), reinterpret_cast<GLvoid*>(offsetof(ImDrawVert, col)));
    }

    void OpenGLImGuiRenderer::RenderWindow(ImGuiViewport* viewport, void*)
    {
        if (!(viewport->Flags & ImGuiViewportFlags_NoRendererClear))
        {
            ImVec4 clear_color = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);
            glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
            glClear(GL_COLOR_BUFFER_BIT);
        }
        RenderStatic(viewport->DrawData);
    }

}
