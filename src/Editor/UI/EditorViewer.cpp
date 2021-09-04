#include "Editor/UI/EditorViewer.h"

#include "Engine.h"
#include "imgui_internal.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "Editor/Editor.h"
#include "ECS/util/Timer.h"
#include "Components/Transform.h"
#include "Editor/ImGui/ImGuizmo.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "Editor/Commands/ViewersCommands.h"

void EditorViewer::Draw()
{
    if (ImGui::BeginDock("Editor viewer", &Active))
    {
        ImVec2 sizeAvail = ImGui::GetContentRegionAvail();
        ImVec2 position = ImGui::GetWindowPos();

        if (sizeAvail.x != 0 && sizeAvail.y != 0)
        {
            ImGui::SetNextItemWidth(110.0f);
            const char* operationsString[] = { "Transform", "Rotate", "Scale" };
            int current = static_cast<u8>(activeOperation);
            ImGui::Combo("##1", &current, operationsString, IM_ARRAYSIZE(operationsString)); ImGui::SameLine();
            activeOperation = static_cast<GizmoOperation>(current);

            ImGui::SetNextItemWidth(80.0f);
            const char* modesString[] = { "Local", "Global" };
            current = static_cast<u8>(activeMode);
            ImGui::Combo("##2", &current, modesString, IM_ARRAYSIZE(modesString));
            activeMode = static_cast<GizmoMode>(current);

            int width, height;
            glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

            moveCamera(glm::vec4(position.x, position.y, sizeAvail.x, sizeAvail.y));
            dragCamera(glm::vec4(position.x, position.y, sizeAvail.x, sizeAvail.y));

            CameraInfo cameraInfo{};

            glm::vec3 up = Camera.orientation * Transform::WorldUp;
            glm::vec3 forward = Camera.orientation * Transform::WorldFront;
            cameraInfo.view = glm::lookAt(Camera.position, Camera.position + forward, up);
            cameraInfo.position = Camera.position;

            f32 aspect = sizeAvail.x / sizeAvail.y;

            if (Camera.projection == Projection::Perspective)
                cameraInfo.projection = glm::perspective(glm::radians(Camera.fov), aspect, Camera.nearClip, Camera.farClip);
            else
                cameraInfo.projection = glm::ortho(0.0f, Camera.ratio * aspect, 0.0f, Camera.ratio * aspect, Camera.nearClip, Camera.farClip);

            showGizmo(cameraInfo, glm::vec4(position.x, position.y, sizeAvail.x, sizeAvail.y));

            GameEngine->GetRenderSystem().SetCustomCameraInfo(cameraInfo);
            GameEngine->GetRenderSystem().Resize(position.x, height - position.y - sizeAvail.y, sizeAvail.x, sizeAvail.y);
            GameEngine->GetRenderSystem().PreUpdate();
            GameEngine->GetRenderSystem().Update();
            GameEngine->GetRenderSystem().PostUpdate();
        }
        else
        {
            int width, height;
            glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
            glViewport(0, 0, width, height);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        }
    }
    else
    {
        int width, height;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);
        glViewport(0, 0, width, height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }
    ImGui::EndDock();
}

void EditorViewer::moveCamera(glm::vec4 windowPosition) noexcept
{
    f64 x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT) &&
        x >= windowPosition.x && x <= windowPosition.z + windowPosition.x &&
        y >= windowPosition.y && y <= windowPosition.w + windowPosition.y)
    {
        f32 dt = ECS::ECS_Engine->GetTimer()->GetNonScaleDeltaTime();
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W))
            Camera.position += Camera.orientation * Transform::WorldFront * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S))
            Camera.position -= Camera.orientation * Transform::WorldFront * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A))
            Camera.position += Camera.orientation * Transform::WorldRight * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D))
            Camera.position -= Camera.orientation * Transform::WorldRight * CameraSpeed * dt;
    }
}

void EditorViewer::dragCamera(glm::vec4 windowPosition) noexcept
{
    f64 x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

    if (x >= windowPosition.x && x <= windowPosition.z + windowPosition.x &&
        y >= windowPosition.y && y <= windowPosition.w + windowPosition.y)
    {
        f64 dX = lastMouseX - x;
        f64 dY = lastMouseY - y;

        if (isFirstClick)
        {
            dX = 0;
            dY = 0;
        }

        lastMouseX = x;
        lastMouseY = y;

        if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT))
        {
            f32 dt = ECS::ECS_Engine->GetTimer()->GetNonScaleDeltaTime();

            if (!CameraInverted)
            {
                cameraAngle.x -= dX * CameraSense * dt;
                cameraAngle.y += dY * CameraSense * dt;
            }
            else
            {
                cameraAngle.x += dX * CameraSense * dt;
                cameraAngle.y -= dY * CameraSense * dt;
            }

            Camera.orientation = glm::quat_cast(glm::yawPitchRoll(glm::radians(cameraAngle.x), glm::radians(cameraAngle.y), glm::radians(cameraAngle.z)));

            isFirstClick = false;
        }
        else
            isFirstClick = true;
    }
    else
        isFirstClick = true;
}

void EditorViewer::showGizmo(const CameraInfo &cameraInfo, glm::vec4 windowPosition)
{
    if (GameEditor->Selected != ECS::INVALID_ENTITY_ID)
    {
        auto *go = static_cast<GameObject *>(ECS::ECS_Engine->GetEntityManager()->GetEntity(GameEditor->Selected));
        if (auto *transform = go->GetComponent<Transform>(); transform != nullptr)
        {
            ImGuizmo::OPERATION operation;
            switch (activeOperation)
            {
            case GizmoOperation::Translation:
                operation = ImGuizmo::TRANSLATE;
                break;

            case GizmoOperation::Rotate:
                operation = ImGuizmo::ROTATE;
                break;

            case GizmoOperation::Scale:
                operation = ImGuizmo::SCALE;
                break;
            }

            ImGuizmo::MODE mode;
            switch (activeMode)
            {
            case GizmoMode::Local:
                mode = ImGuizmo::LOCAL;
                break;

            case GizmoMode::Global:
                mode = ImGuizmo::WORLD;
                break;
            }

            ImGuizmo::SetOrthographic(Camera.projection == Projection::Orthographic);
            ImGuizmo::BeginFrame();
            glm::mat4 model = transform->GetMat();
            ImGuizmo::SetRect(windowPosition.x, windowPosition.y, windowPosition.z, windowPosition.w);
            ImGuizmo::Manipulate(glm::value_ptr(cameraInfo.view), glm::value_ptr(cameraInfo.projection), operation, mode, &model[0][0]);

            if (model != transform->GetMat())
            {
                transform->GetMat() = model;
                glm::vec3 sizeScale;
                glm::quat orientation;
                glm::vec3 position;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(model, sizeScale, orientation, position, skew, perspective);

                Position pos;
                pos.position = position;
                pos.rotate = orientation;
                pos.scale = sizeScale;

                if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                    lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<Transform>>(go);

                if (transform->IsLocalPosition())
                {
                    transform->SetGlobalPos(pos);
                    transform->SetLocalPos(transform->GetLocalPos());
                }
                else
                    transform->SetGlobalPos(pos);
            }
        }
    }
}
