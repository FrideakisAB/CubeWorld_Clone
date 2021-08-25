#include "Editor/UI/Viewers/TransformViewer.h"

#include "Utils/Mathf.h"
#include "Editor/Editor.h"
#include <glm/gtc/type_ptr.hpp>
#include "Components/Transform.h"
#include "Editor/Commands/ViewersCommands.h"

void TransformViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &transform = dynamic_cast<Transform&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Transform", &closed))
    {
        bool local;
        bool update = false;

        Position pos;

        if (ImGui::TreeNode("Local position"))
        {
            local = true;
            pos = transform.GetLocalPos();

            glm::vec3 secRotate = glm::eulerAngles(pos.rotate);
            glm::vec3 secChange = glm::degrees(secRotate);

            if (ImGui::InputFloat3("Position", glm::value_ptr(pos.position)))
                update = true;
            if (ImGui::InputFloat3("Rotation (YPR)", glm::value_ptr(secChange)))
                update = true;
            if (ImGui::InputFloat3("Scale", glm::value_ptr(pos.scale)))
                update = true;

            glm::vec3 postRotate = glm::radians(secChange);
            if (!Mathf::Approximately(postRotate, secRotate))
            {
                auto fg = glm::quat(postRotate);
                pos.rotate = fg;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Global position"))
        {
            local = false;
            pos = transform.GetGlobalPos();

            glm::vec3 secRotate = glm::eulerAngles(pos.rotate);
            glm::vec3 secChange = glm::degrees(secRotate);

            if (ImGui::InputFloat3("Position", glm::value_ptr(pos.position)))
                update = true;
            if (ImGui::InputFloat3("Rotation (YPR)", glm::value_ptr(secChange)))
                update = true;
            if (ImGui::InputFloat3("Scale", glm::value_ptr(pos.scale)))
                update = true;

            glm::vec3 postRotate = glm::radians(secChange);
            if (!Mathf::Approximately(postRotate, secRotate))
            {
                auto fg = glm::quat(postRotate);
                pos.rotate = fg;
            }

            ImGui::TreePop();
        }

        if (update)
        {
            GameEditor->CommandList.AddCommand<ChangeState<Transform>>(&go);

            if (local)
                transform.SetLocalPos(pos);
            else
                transform.SetGlobalPos(pos);

            GameEditor->CommandList.Redo();
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<Transform>>(&go);
        GameEditor->CommandList.Redo();
    }
}
