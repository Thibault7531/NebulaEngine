#include "SceneRenderer.h"

#include "Renderer2D.h"
#include "Nebula/CoreSystems/Application/Application.h"
#include "Nebula/Scene/Components.h"
#include "Nebula/Scene/Entity.h"
#include "Platform/Nebula/CoreSystems/Windows/WindowManager.h"

namespace Nebula {

	SceneRenderer::SceneData* SceneRenderer::m_SceneData = new SceneRenderer::SceneData();

	void SceneRenderer::Init()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Initializing Nebula Renderer");

		auto [width, height] = Platform::WindowManager::Get().GetMainWindow()->GetClientSize();
		m_SceneData->m_Width = width;
		m_SceneData->m_Height = height;

		m_SceneData->m_Device = Application::Get().GetDevice();
	}

	void SceneRenderer::Shutdown()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Shutting down Nebula Renderer");
	}

	void SceneRenderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		m_SceneData->m_Width = width;
		m_SceneData->m_Height = height;
	}

	void SceneRenderer::RenderScene(Ref<Scene> scene)
	{
		RenderScene(scene, Application::Get().GetMainWindowSwapchain()->GetCurrentFrame());
	}

	void SceneRenderer::RenderScene(Ref<Scene> scene, Ref<Graphics::Framebuffer> framebuffer)
	{
		Entity cameraEntity = scene->GetPrimaryCameraEntity();
		if (cameraEntity) {
			auto camera = cameraEntity.GetComponent<CameraComponent>().Camera;
			glm::mat4 transform = cameraEntity.GetComponent<TransformComponent>().GetTransform();

			RenderSprites(scene, camera, transform, framebuffer);
		}
	}

	void SceneRenderer::RenderSceneEditor(Ref<Scene> scene, const EditorCamera& camera)
	{
		RenderSceneEditor(scene, camera, Application::Get().GetMainWindowSwapchain()->GetCurrentFrame());
	}

	void SceneRenderer::RenderSceneEditor(Ref<Scene> scene, const EditorCamera& camera,
		Ref<Graphics::Framebuffer> framebuffer)
	{
		RenderSpritesEditor(scene, camera, framebuffer);
	}

	void SceneRenderer::RenderSprites(Ref<Scene> scene, const Camera& camera, const glm::mat4& cameraTransform, Ref<Graphics::Framebuffer> framebuffer)
	{
		entt::registry& registry = scene->GetRegistry();

		Renderer2D::BeginScene(camera, cameraTransform, framebuffer, true);

		for (const auto view = registry.view<TransformComponent, SpriteRendererComponent>(); const auto entity : view)
		{
			auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad({.transform = transform.GetTransform(), .texture = sprite.Texture, .color = sprite.Color, .tilingFactor = sprite.TilingFactor});
		}

		Renderer2D::EndScene();
	}

	void SceneRenderer::RenderSpritesEditor(Ref<Scene> scene, const EditorCamera& camera,
		Ref<Graphics::Framebuffer> framebuffer)
	{
		entt::registry& registry = scene->GetRegistry();

		Renderer2D::BeginScene(camera, framebuffer, true);

		for (const auto view = registry.view<TransformComponent, SpriteRendererComponent>(); const auto entity : view)
		{
			auto [transform, sprite] = view.get<TransformComponent, SpriteRendererComponent>(entity);

			Renderer2D::DrawQuad({.transform = transform.GetTransform(), .texture = sprite.Texture, .color = sprite.Color, .tilingFactor = sprite.TilingFactor, .entityID = static_cast<int32_t>(entity)});
		}

		Renderer2D::EndScene();
	}
}
