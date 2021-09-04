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
        bool update = false;

        bool isLocal = transform.IsLocalPosition();
        ImGui::Checkbox("Local", &isLocal);
        if (isLocal != transform.IsLocalPosition())
            update = true;

        Position position;
        if (isLocal)
            position = transform.GetLocalPos();
        else
            position = transform.GetGlobalPos();

        glm::vec3 currentRotate = glm::eulerAngles(position.rotate);
        glm::vec3 changeRotate = glm::degrees(currentRotate);

        if (ImGui::InputFloat3("Position", glm::value_ptr(position.position)))
            update = true;
        if (ImGui::InputFloat3("Rotation (YPR)", glm::value_ptr(changeRotate)))
            update = true;
        if (ImGui::InputFloat3("Scale", glm::value_ptr(position.scale)))
            update = true;

        glm::vec3 postRotate = glm::radians(changeRotate);
        if (!Mathf::Approximately(postRotate, currentRotate))
            position.rotate = glm::quat(postRotate);

        if (update)
        {
            if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<Transform>>(&go);

            if (isLocal)
                transform.SetLocalPos(position);
            else
                transform.SetGlobalPos(position);
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<Transform>>(&go);
        GameEditor->CommandList.Redo();
    }
}
