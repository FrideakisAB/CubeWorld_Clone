#include "Editor/UI/EditorViewer.h"

#include "Engine.h"
#include "imgui_internal.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "ECS/util/Timer.h"
#include "Systems/RenderSystem.h"
#include "Components/Transform.h"
#include <glm/gtx/euler_angles.hpp>

void EditorViewer::Draw()
{
    if (ImGui::BeginDock("Editor viewer", &Active))
    {
        ImVec2 sizeAvail = ImGui::GetContentRegionAvail();
        ImVec2 position = ImGui::GetWindowPos();

        if (sizeAvail.x != 0 && sizeAvail.y != 0)
        {
            int width, height;
            glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

            ImGuiContext *g = ImGui::GetCurrentContext();
            if (g->ActiveIdWindow == ImGui::GetCurrentWindow())
            {
                MoveCamera();
                DragCamera();
            }
            else
                isFirstClick = true;

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

void EditorViewer::MoveCamera() noexcept
{
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT))
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

void EditorViewer::DragCamera() noexcept
{
    f64 x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

    f64 dX = lastMouseX - x;
    f64 dY = lastMouseY - y;

    if (isFirstClick)
    {
        dX = 0;
        dY = 0;
    }

    lastMouseX = x;
    lastMouseY = y;

    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_LEFT))
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
