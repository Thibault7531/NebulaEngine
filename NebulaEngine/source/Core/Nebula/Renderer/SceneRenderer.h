#pragma once

#include "Model/Model.h"
#include "Nebula/Graphics/CommandBuffer.h"
#include "Nebula/Graphics/Device.h"
#include "Nebula/Scene/Scene.h"

namespace Nebula {

	class SceneRenderer
	{
	public:
		static void Init();
		static void Shutdown();
		static void OnWindowResize(uint32_t width, uint32_t height);

		static void RenderScene(Ref<Scene> scene);
		static void RenderScene(Ref<Scene> scene, Ref<Graphics::Framebuffer> framebuffer);
		static void RenderSceneEditor(Ref<Scene> scene, const EditorCamera& camera);
		static void RenderSceneEditor(Ref<Scene> scene, const EditorCamera& camera, Ref<Graphics::Framebuffer> framebuffer);

	private:
		static void RenderSprites(Ref<Scene> scene, const Camera& camera, const glm::mat4& cameraTransform, Ref<Graphics::Framebuffer> framebuffer);
		static void RenderSpritesEditor(Ref<Scene> scene, const EditorCamera& camera, Ref<Graphics::Framebuffer> framebuffer);

	private:
		struct SceneData
		{
			uint32_t m_Width, m_Height;
			Ref<Graphics::Device> m_Device;
		};

		static SceneData* m_SceneData;
	};

}
