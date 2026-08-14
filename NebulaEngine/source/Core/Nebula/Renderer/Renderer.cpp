#include "Renderer.h"

#include "Renderer2D.h"
#include "WindowManager.h"
#include "gtc/type_ptr.hpp"
#include "Nebula/CoreSystems/Application/Application.h"

namespace Nebula {

	Renderer::SceneData* Renderer::m_SceneData = new Renderer::SceneData();

	void Renderer::Init()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Initializing Nebula Renderer");

		auto [width, height] = Platform::WindowManager::Get().GetMainWindow()->GetClientSize();
		m_SceneData->m_Width = width;
		m_SceneData->m_Height = height;

		m_SceneData->m_Device = Application::Get().GetDevice();
		m_SceneData->m_UBO = m_SceneData->m_Device->CreateUniformBuffer({
			{ Graphics::ShaderDataType::Matrix4, "u_ViewProjection" },
			{ Graphics::ShaderDataType::Matrix4, "u_Transform" }
		});
	}

	void Renderer::Shutdown()
	{
		NB_PROFILE_FUNCTION();

		NB_CORE_INFO("Shutting down Nebula Renderer");

		m_SceneData->m_UBO.reset();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		m_SceneData->m_Width = width;
		m_SceneData->m_Height = height;
	}

	void Renderer::BeginScene(const PerspectiveCamera& camera)
	{
		SetRenderState();
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_SceneData->m_UBO->SetData(0, glm::value_ptr(m_SceneData->viewProjectionMatrix));
	}

	void Renderer::BeginScene(const OrthographicCamera& camera)
	{
		SetRenderState();
		m_SceneData->viewProjectionMatrix = camera.getViewProjectionMatrix();
		m_SceneData->m_UBO->SetData(0, glm::value_ptr(m_SceneData->viewProjectionMatrix));
	}

	void Renderer::EndScene()
	{
		m_SceneData->m_RenderPass->EndRenderPass();
		m_SceneData->m_CommandBuffer->Commit();
	}

	void Renderer::Submit(const Ref<Graphics::ShaderPipeline>& shader, const Ref<Graphics::VertexArray>& vertexArray, const glm::mat4& transform, const std::map<std::string, Ref<Graphics::UniformBuffer>>& uniformBuffers)
	{
		m_SceneData->m_RenderPass->SetShaderPipeline(shader);
		m_SceneData->m_UBO->SetData(1, glm::value_ptr(transform));
		m_SceneData->m_RenderPass->SetUniformBlock("RendererUBO", m_SceneData->m_UBO);
		for (auto& location : uniformBuffers)
		{
			m_SceneData->m_RenderPass->SetUniformBlock(location.first, location.second);
		}
		m_SceneData->m_RenderPass->DrawArrayIndexed(vertexArray);
	}

	void Renderer::SubmitModel(const Ref<Graphics::ShaderPipeline>& shader, const Ref<Model>& model, const glm::mat4& transform)
	{
		m_SceneData->m_RenderPass->SetShaderPipeline(shader);
		m_SceneData->m_UBO->SetData(1, glm::value_ptr(transform));
		m_SceneData->m_RenderPass->SetUniformBlock("RendererUBO", m_SceneData->m_UBO);

		for (const Ref<Mesh>& mesh : model->GetMeshes())
		{
			m_SceneData->m_RenderPass->DrawArrayIndexed(mesh->GetVertexArray());
		}
	}

	void Renderer::SetRenderState()
	{
		m_SceneData->m_CommandBuffer = m_SceneData->m_Device->CreateCommandBuffer();
		m_SceneData->m_RenderPass = m_SceneData->m_CommandBuffer->CreateRenderPass(Application::Get().GetMainFramebuffer(), true);
		m_SceneData->m_RenderPass->SetCullMode(Graphics::CullMode::Back);
		m_SceneData->m_RenderPass->SetDepthStencilState({.depthCompareFunction = Graphics::DepthCompareFunction::Less, .depthWrite = true});
		m_SceneData->m_RenderPass->SetViewport(0, 0, m_SceneData->m_Width, m_SceneData->m_Height, 0.1f, 100.f);
	}
}
