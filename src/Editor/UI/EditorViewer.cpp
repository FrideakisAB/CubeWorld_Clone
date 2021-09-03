#include "Editor/UI/EditorViewer.h"

#include "Engine.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "Systems/RenderSystem.h"

void EditorViewer::Draw()
{
    if (ImGui::BeginDock("Editor viewer", &Active))
    {
        ImVec2 sizeAvail = ImGui::GetContentRegionAvail();
        ImVec2 position = ImGui::GetWindowPos();

        int width, height;
        glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

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
    ImGui::EndDock();
}
