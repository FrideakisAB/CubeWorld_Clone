#include "Editor/UI/Viewers/CameraViewer.h"

#include "Editor/Editor.h"
#include "Components/Camera.h"
#include "Editor/Commands/ViewersCommands.h"


void CameraViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &camera = dynamic_cast<Camera&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Camera", &closed))
    {
        bool update = false;

        float fovOrRatio;
        if (camera.Proj == Projection::Perspective)
        {
            fovOrRatio = camera.Fov;
            ImGui::InputFloat("Fov", &fovOrRatio);
            if (fovOrRatio != camera.Fov)
                update = true;
        }
        else
        {
            fovOrRatio = camera.Ratio;
            ImGui::InputFloat("Ratio", &fovOrRatio);
            if (fovOrRatio != camera.Ratio)
                update = true;
        }

        float nearClip = camera.NearClip;
        ImGui::InputFloat("Near clip", &nearClip);
        if (nearClip != camera.NearClip)
            update = true;

        float farClip = camera.FarClip;
        ImGui::InputFloat("Far clip", &farClip);
        if (farClip != camera.FarClip)
            update = true;

        const char* items[] = { "Perspective", "Orthographic" };
        int item_current = static_cast<u8>(camera.Proj);
        ImGui::Combo("Projection", &item_current, items, IM_ARRAYSIZE(items));
        if (item_current != static_cast<u8>(camera.Proj))
            update = true;

        if(update)
        {
            GameEditor->CommandList.AddCommand<ChangeState<Camera>>(&go);

            if (camera.Proj == Projection::Perspective)
                camera.Fov = fovOrRatio;
            else
                camera.Ratio = fovOrRatio;

            camera.NearClip = nearClip;
            camera.FarClip = farClip;

            camera.Proj = static_cast<Projection>((u8)item_current);

            GameEditor->CommandList.Redo();
        }
    }
    if (!closed)
    {
        if (Camera::Main == &camera)
        {
            GameEditor->CommandList.AddCommand<SetCamera>(nullptr);
            GameEditor->CommandList.Redo();
        }

        GameEditor->CommandList.AddCommand<DeleteComponent<Camera>>(&go);
        GameEditor->CommandList.Redo();
    }
}