#include "SceneHierarchyPanel.h"

#include <imgui.h>

#include "gtc/type_ptr.hpp"
#include "Nebula/Scene/Components.h"

namespace Nebula
{

    SceneHierarchyPanel::SceneHierarchyPanel(const Ref<Scene>& context)
    {
        SetContext(context);
    }

    void SceneHierarchyPanel::SetContext(const Ref<Scene>& context)
    {
        m_Context = context;
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");
        {
            for (auto entityID : m_Context->m_Registry.view<entt::entity>())
            {
                if (entityID == entt::null || static_cast<int>(entityID) == 0) break;

                Entity entity{entityID, m_Context.get()};

                DrawEntityNode(entity);
            }
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) m_SelectedEntity = {};

        ImGui::End();

        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity) DrawComponents(m_SelectedEntity);
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding;
        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        if (opened)
        {
            ImGui::TreePop();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256] = {};
            strcpy_s(buffer, sizeof(buffer), tag.c_str());
            if (ImGui::InputText("Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        if (entity.HasComponent<TransformComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(TransformComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Transform"))
            {
                auto& transform = entity.GetComponent<TransformComponent>().Transform;
                ImGui::DragFloat3("Position", glm::value_ptr(transform[3]), .1f);

                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<CameraComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(CameraComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Camera"))
            {
                auto& cameraComponent = entity.GetComponent<CameraComponent>();
                auto& camera = cameraComponent.Camera;

                ImGui::Checkbox("Primary", &cameraComponent.Primary);

                const char* projectionTypes[] = { "Perspective", "Orthographic" };
                const char* currentProjectionType = projectionTypes[static_cast<int>(camera.GetProjectionType())];

                if (ImGui::BeginCombo("Projection", currentProjectionType))
                {
                    for (int i = 0; i < 2; i++)
                    {
                        bool isSelected = (currentProjectionType == projectionTypes[i]);
                        if (ImGui::Selectable(projectionTypes[i], isSelected))
                        {
                            currentProjectionType = projectionTypes[i];
                            camera.SetProjectionType(static_cast<SceneCamera::ProjectionType>(i));
                        }

                        if (isSelected) ImGui::SetItemDefaultFocus();
                    }

                    ImGui::EndCombo();
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Perspective)
                {
                    float verticalFOV = glm::degrees(camera.GetPerspectiveVerticalFOV());
                    if (ImGui::DragFloat("FOV", &verticalFOV))
                    {
                        camera.SetPerspectiveVerticalFOV(glm::radians(verticalFOV));
                    }

                    float nearClip = camera.GetPerspectiveNearClip();
                    if (ImGui::DragFloat("Near Clip", &nearClip))
                    {
                        camera.SetPerspectiveNearClip(nearClip);
                    }

                    float farClip = camera.GetPerspectiveFarClip();
                    if (ImGui::DragFloat("Far Clip", &farClip))
                    {
                        camera.SetPerspectiveFarClip(farClip);
                    }
                }

                if (camera.GetProjectionType() == SceneCamera::ProjectionType::Orthographic)
                {
                    float size = camera.GetOrthographicSize();
                    if (ImGui::DragFloat("Size", &size))
                    {
                        camera.SetOrthographicSize(size);
                    }

                    float nearClip = camera.GetOrthographicNearClip();
                    if (ImGui::DragFloat("Near Clip", &nearClip))
                    {
                        camera.SetOrthographicNearClip(nearClip);
                    }

                    float farClip = camera.GetOrthographicFarClip();
                    if (ImGui::DragFloat("Far Clip", &farClip))
                    {
                        camera.SetOrthographicFarClip(farClip);
                    }

                    ImGui::Checkbox("Fixed Aspect Ratio", &cameraComponent.FixedAspectRatio);
                }

                ImGui::TreePop();
            }
        }

        if (entity.HasComponent<SpriteRendererComponent>())
        {
            if (ImGui::TreeNodeEx((void*)typeid(SpriteRendererComponent).hash_code(), ImGuiTreeNodeFlags_DefaultOpen, "Sprite Renderer"))
            {
                auto& src = entity.GetComponent<SpriteRendererComponent>();

                ImGui::ColorEdit4("Color", glm::value_ptr(src.Color));

                ImGui::TreePop();
            }
        }
    }

}
