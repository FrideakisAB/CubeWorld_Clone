#include "Editor/UI/Viewers/MeshViewer.h"

#include "Editor/Editor.h"
#include "Components/MeshComponent.h"
#include "Editor/ImGui/imgui_custom.h"
#include "Editor/Commands/ViewersCommands.h"

void MeshViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &mesh = dynamic_cast<MeshComponent&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Mesh component", &closed))
    {
        bool update = false;

        std::string context;
        CustomTextState state;

        if (!mesh.IsValid())
        {
            if (mesh.GetAsset())
            {
                context = mesh.GetAsset()->GetName();
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
            context = mesh.GetAsset()->GetName();
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
                if (GameEngine->GetAssetsManager().GetAsset<Mesh>(str.data()) != nullptr)
                {
                    update = true;
                    asset = str;
                }
                ImGui::EndDragDropTarget();
            }
        };

        if (ImGui::TextHandleButton("Mesh", context, "Mesh", state, 16, dragCollector))
            ImGui::OpenPopup("MeshSelector");

        auto isMeshFunction = [](const AssetsHandle &handle) {
            return dynamic_cast<Mesh*>(handle.get()) != nullptr;
        };

        if (ImGui::AssetSelectorPopup("MeshSelector", context, "Mesh", state, asset, isMeshFunction))
            update = true;

        if (update)
        {
            GameEditor->CommandList.AddCommand<SetMesh>(&go, asset);
            GameEditor->CommandList.Redo();
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<MeshComponent>>(&go);
        GameEditor->CommandList.Redo();
    }
}
