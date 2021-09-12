#include "Editor/UI/Viewers/CameraViewer.h"

#include "Editor/Editor.h"
#include "Components/Camera.h"
#include "Editor/ImGui/ImCustom.h"
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

        std::string context;
        CustomTextState state;

        if (!camera.IsValidSkybox())
        {
            if (camera.GetAsset())
            {
                context = camera.GetAsset()->GetName();
                if (context.empty())
                    context = "(custom)";
                state = CustomTextState::Invalid;
            }
            else
            {
                context = "(no select)";
                state = CustomTextState::None;
            }
        }
        else
        {
            context = camera.GetAsset()->GetName();
            state = CustomTextState::Global;
            if (context.empty())
            {
                context = "(custom)";
                state = CustomTextState::NoGlobal;
            }
        }

        std::string asset;
        auto dragCollector = [&](){
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
            {
                std::string_view str = *static_cast<std::string_view*>(payload->Data);
                if (auto *tex = GameEngine->GetAssetsManager().GetAsset<Texture>(str.data()); tex != nullptr)
                {
                    if (tex->GetType() == TexType::TextureCube)
                    {
                        update = true;
                        asset = str;
                    }
                }
                ImGui::EndDragDropTarget();
            }
        };

        if (ImGui::TextHandleButton("Skybox", context, "Mesh", state, 16, dragCollector))
            ImGui::OpenPopup("SkyboxSelector");

        auto isSamplerCubeFunction = [](const AssetsHandle &handle) {
            auto *tex = dynamic_cast<Texture*>(handle.get());
            return tex != nullptr && tex->GetType() == TexType::TextureCube;
        };
        if (ImGui::AssetSelectorPopup("SkyboxSelector", context, "Mesh", state, asset, isSamplerCubeFunction))
            update = true;

        if(update)
        {
            if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<Camera>>(&go);

            if (camera.Proj == Projection::Perspective)
                camera.Fov = fovOrRatio;
            else
                camera.Ratio = fovOrRatio;

            camera.NearClip = nearClip;
            camera.FarClip = farClip;

            camera.Proj = static_cast<Projection>((u8)item_current);

            if (!asset.empty())
                camera.SetSkybox(GameEngine->GetAssetsManager().GetAsset(asset));
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