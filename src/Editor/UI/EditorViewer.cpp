#include "Editor/UI/EditorViewer.h"

#include "Engine.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "Systems/RenderSystem.h"
#include "Components/Transform.h"

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

            CameraInfo cameraInfo{};

            glm::vec3 up = Camera.orientation * Transform::WorldUp;
            glm::vec3 forward = Camera.orientation * Transform::WorldFront;
            cameraInfo.view = glm::lookAt(Camera.position, Camera.position + forward, up);

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
