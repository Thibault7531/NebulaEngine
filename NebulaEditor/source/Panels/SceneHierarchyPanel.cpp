#include "SceneHierarchyPanel.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <gtc/type_ptr.hpp>

#include "Nebula/ImGui/ImGuiLayer.h"
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
        m_SelectedEntity = {};
    }

    void SceneHierarchyPanel::OnImGuiRender()
    {
        ImGui::Begin("Scene Hierarchy");
        {
            for (auto entityID : m_Context->m_Registry.view<entt::entity>())
            {
                if (entityID == entt::null) break;

                Entity entity{entityID, m_Context.get()};

                DrawEntityNode(entity);
            }
        }

        if (ImGui::IsMouseDown(0) && ImGui::IsWindowHovered()) m_SelectedEntity = {};

        ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverItems | ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_MouseButtonRight;
        if (ImGui::BeginPopupContextWindow(nullptr, popupFlags))
        {
            if (ImGui::MenuItem("Create Empty Entity")) m_Context->CreateEntity("Empty Entity");
            ImGui::EndPopup();
        }

        ImGui::End();

        ImGui::Begin("Properties");
        {
            if (m_SelectedEntity)
            {
                DrawComponents(m_SelectedEntity);
            }
        }
        ImGui::End();
    }

    void SceneHierarchyPanel::DrawEntityNode(Entity entity)
    {
        auto& tag = entity.GetComponent<TagComponent>().Tag;

        ImGuiTreeNodeFlags flags = ((m_SelectedEntity == entity) ? ImGuiTreeNodeFlags_Selected : 0) | ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_SpanAvailWidth;
        bool opened = ImGui::TreeNodeEx(reinterpret_cast<void*>(static_cast<uint64_t>(static_cast<uint32_t>(entity))), flags, "%s", tag.c_str());
        if (ImGui::IsItemClicked())
        {
            m_SelectedEntity = entity;
        }

        bool entityDeleted = false;
        ImGuiPopupFlags popupFlags = ImGuiPopupFlags_NoOpenOverExistingPopup | ImGuiPopupFlags_MouseButtonRight;
        if (ImGui::BeginPopupContextItem(nullptr, popupFlags))
        {
            if (ImGui::MenuItem("Delete Entity")) entityDeleted = true;
            ImGui::EndPopup();
        }

        if (opened)
        {
            ImGui::TreePop();
        }

        if (entityDeleted)
        {
            if (m_SelectedEntity == entity) m_SelectedEntity = {};
            m_Context->DestroyEntity(entity);
        }
    }

    static void DrawVec3Control(const std::string& label, glm::vec3& values, float resetValue = 0.0f, float columnWidth = 100.0f)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = ImGuiLayer::s_Fonts.Bold;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        {
            ImGui::SetColumnWidth(0, columnWidth);
            {
                ImGui::Text("%s", label.c_str());
            }
            ImGui::NextColumn();
            {
                ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
                ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(5.0f, 5.0f));

                float lineHeight = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
                ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.8f, 0.1f, 0.15f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.9f, 0.2f, 0.2f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.8f, 0.1f, 0.15f, 1.0f });
                ImGui::PushFont(boldFont);
                if (ImGui::Button("X", buttonSize))
                    values.x = resetValue;
                ImGui::PopFont();
                ImGui::PopStyleColor(3);

                ImGui::SameLine();
                ImGui::DragFloat("##X", &values.x, .1f, 0, 0, "%.2f");
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.2f, 0.7f, 0.2f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.3f, 0.8f, 0.3f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.2f, 0.7f, 0.2f, 1.0f });
                ImGui::PushFont(boldFont);
                if (ImGui::Button("Y", buttonSize))
                    values.y = resetValue;
                ImGui::PopFont();
                ImGui::PopStyleColor(3);

                ImGui::SameLine();
                ImGui::DragFloat("##Y", &values.y, .1f, 0, 0, "%.2f");
                ImGui::PopItemWidth();
                ImGui::SameLine();

                ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{0.1f, 0.25f, 0.8f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{0.2f, 0.35f, 0.9f, 1.0f });
                ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{0.1f, 0.25f, 0.8f, 1.0f });
                ImGui::PushFont(boldFont);
                if (ImGui::Button("Z", buttonSize))
                    values.z = resetValue;
                ImGui::PopFont();
                ImGui::PopStyleColor(3);

                ImGui::SameLine();
                ImGui::DragFloat("##Z", &values.z, .1f, 0, 0, "%.2f");
                ImGui::PopItemWidth();

                ImGui::PopStyleVar();
            }
        }
        ImGui::Columns(1);

        ImGui::PopID();
    }

    template<typename T, typename UIFonction>
    static void DrawComponent(const std::string& name, Entity entity, UIFonction uiFunction)
    {
        ImGuiIO& io = ImGui::GetIO();
        auto boldFont = ImGuiLayer::s_Fonts.Bold;
        const ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_Framed | ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding | ImGuiTreeNodeFlags_AllowItemOverlap;

        if (entity.HasComponent<T>())
        {
            auto& component = entity.GetComponent<T>();
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            ImGui::PushFont(boldFont);
            float lineHeigth = GImGui->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            bool open = ImGui::TreeNodeEx((void*)typeid(T).hash_code(), flags, "%s", name.c_str());

            ImGui::SameLine(contentRegionAvail.x - lineHeigth * .5f + GImGui->Style.FramePadding.x * .5f);
            if (ImGui::Button("+", {lineHeigth, lineHeigth}))
            {
                ImGui::OpenPopup("ComponentSettings");
            }
            ImGui::PopFont();

            bool removeComponent = false;
            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::MenuItem("Delete Component")) removeComponent = true;

                ImGui::EndPopup();
            }

            if (open)
            {
                uiFunction(component);

                ImGui::TreePop();
            }

            if (removeComponent) entity.RemoveComponent<T>();
        }
    }

    void SceneHierarchyPanel::DrawComponents(Entity entity)
    {
        if (entity.HasComponent<TagComponent>())
        {
            auto& tag = entity.GetComponent<TagComponent>().Tag;

            char buffer[256] = {};
            strcpy(buffer, tag.c_str());
            if (ImGui::InputText("##Tag", buffer, sizeof(buffer)))
            {
                tag = std::string(buffer);
            }
        }

        ImGui::SameLine();
        ImGui::PushItemWidth(-1);

        if (ImGui::Button("Add Component")) ImGui::OpenPopup("AddComponent");

        if (ImGui::BeginPopup("AddComponent"))
        {
            if (ImGui::MenuItem("Camera"))
            {
                m_SelectedEntity.AddComponent<CameraComponent>();
                ImGui::CloseCurrentPopup();
            }

            if (ImGui::MenuItem("Sprite Renderer"))
            {
                m_SelectedEntity.AddComponent<SpriteRendererComponent>();
                ImGui::CloseCurrentPopup();
            }

            ImGui::EndPopup();
        }

        ImGui::PopItemWidth();

        DrawComponent<TransformComponent>("Transform", entity, [](auto& component)
        {
            DrawVec3Control("Translation", component.Translation);
            glm::vec3 rotation = glm::degrees(component.Rotation);
            DrawVec3Control("Rotation", rotation);
            component.Rotation = glm::radians(rotation);
            DrawVec3Control("Scale", component.Scale, 1.f);
        });

        DrawComponent<CameraComponent>("Camera", entity, [](auto& component)
        {
            auto& camera = component.Camera;

                ImGui::Checkbox("Primary", &component.Primary);

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

                    ImGui::Checkbox("Fixed Aspect Ratio", &component.FixedAspectRatio);
                }
        });

        DrawComponent<SpriteRendererComponent>("Sprite Renderer", entity, [](auto& component)
        {
            ImGui::ColorEdit4("Color", glm::value_ptr(component.Color));
        });
    }

}
