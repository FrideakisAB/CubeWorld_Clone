#include "Editor/UI/LightingWindow.h"

#include "Engine.h"
#include "Editor/Editor.h"
#include "Render/Lighting.h"
#include "Editor/Commands/ViewersCommands.h"

void LightingWindow::Draw()
{
    if (ImGui::BeginDock("Lighting", &Active))
    {
        bool update = false;

        f32 ambient = GameEngine->GetLighting().Ambient;
        ImGui::InputFloat("Ambient", &ambient);
        if (ambient != GameEngine->GetLighting().Ambient)
            update = true;

        f32 shadowsPower = GameEngine->GetLighting().ShadowsPower;
        ImGui::InputFloat("Shadows power", &shadowsPower);
        if (shadowsPower != GameEngine->GetLighting().ShadowsPower)
            update = true;

        f32 exposure = GameEngine->GetLighting().Exposure;
        ImGui::InputFloat("Exposure", &exposure);
        if (exposure != GameEngine->GetLighting().Exposure)
            update = true;

        if (update)
        {
            GameEditor->CommandList.AddCommand<UpdateLighting>(ambient, shadowsPower, exposure);
            GameEditor->CommandList.Redo();
        }
    }
    ImGui::EndDock();
}
