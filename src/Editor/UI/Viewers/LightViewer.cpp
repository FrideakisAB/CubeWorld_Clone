#include "Editor/UI/Viewers/LightViewer.h"

#include "Editor/Editor.h"
#include "Components/LightSource.h"
#include "Editor/Commands/ViewersCommands.h"

void LightViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &light = dynamic_cast<LightSource&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Light source", &closed))
    {
        bool update = false;

        const char* items[] = { "Directional", "Point", "Spot" };
        int item_current = static_cast<u8>(light.Type);
        ImGui::Combo("Light type", &item_current, items, IM_ARRAYSIZE(items));
        if (item_current != static_cast<u8>(light.Type))
            update = true;

        glm::vec3 color = light.Color;
        ImGui::ColorEdit3("Color", &color[0], ImGuiColorEditFlags_NoInputs);
        if(color != light.Color)
            update = true;

        float radius = light.Radius;
        ImGui::InputFloat("Radius", &radius);
        if (radius != light.Radius)
            update = true;

        float intensity = light.Intensity;
        ImGui::InputFloat("Intensity", &intensity);
        if (intensity != light.Intensity)
            update = true;

        float cutterOff = light.CutterOff;
        if (light.Type == LightType::Spot)
        {
            ImGui::InputFloat("Cutter off angle", &cutterOff);
            if (cutterOff != light.CutterOff)
                update = true;
        }

        if (update)
        {
            GameEditor->CommandList.AddCommand<ChangeState<LightSource>>(&go);

            light.Type = static_cast<LightType>((u8)item_current);
            light.Color = color;
            light.Radius = radius;
            light.Intensity = intensity;
            light.CutterOff = cutterOff;

            GameEditor->CommandList.Redo();
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<LightSource>>(&go);
        GameEditor->CommandList.Redo();
    }
}
