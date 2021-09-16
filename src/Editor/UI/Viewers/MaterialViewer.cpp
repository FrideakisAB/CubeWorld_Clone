#include "Editor/UI/Viewers/MaterialViewer.h"

#include "Editor/Editor.h"
#include "Render/Texture.h"
#include "Systems/RenderSystem.h"
#include "Editor/ImGui/ImCustom.h"
#include "Components/MaterialComponent.h"
#include "Editor/Commands/ViewersCommands.h"
#include "Editor/UI/Viewers/MaterialAssetViewer.h"

void MaterialViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &material = dynamic_cast<MaterialComponent&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Material component", &closed))
    {
        bool update = false;

        std::string context;
        CustomTextState state;

        if (!material.IsValid())
        {
            context = "(no select)";
            state = CustomTextState::None;
        }
        else
        {
            context = material.GetMaterial()->GetName();
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
                if (GameEngine->GetAssetsManager().GetAsset<Material>(str.data()) != nullptr)
                {
                    update = true;
                    asset = str;
                }
                ImGui::EndDragDropTarget();
            }
        };

        auto dragSource = [&]() {
            if (context == "(custom)")
            {
                MaterialComponent *matPtr = &material;
                ImGui::SetDragDropPayload("SCENE_MATERIAL", &matPtr, sizeof(MaterialComponent*));
                ImGui::Text("Local material asset");
            }
        };

        if (ImGui::TextHandleButton("Material", context, "Material", state, 16, dragCollector, dragSource))
            ImGui::OpenPopup("MaterialSelector");

        auto isMaterialFunction = [](const AssetsHandle &handle) {
            return dynamic_cast<Material*>(handle.get()) != nullptr;
        };
        if (ImGui::AssetSelectorPopup("MaterialSelector", context, "Material", state, asset, isMaterialFunction))
            update = true;

        if (update)
        {
            GameEditor->CommandList.AddCommand<SetMaterial>(&go, asset);
            GameEditor->CommandList.Redo();
        }

        bool makeCustom = false;
        if (state != CustomTextState::None && state != CustomTextState::NoGlobal)
        {
            ImGui::Text("To edit the material, you need a local copy");
            if (ImGui::Button("Make as custom"))
                makeCustom = true;
        }
        else if (state == CustomTextState::None)
            if (ImGui::Button("Make custom"))
                makeCustom = true;

        if (makeCustom)
        {
            if (state != CustomTextState::None)
            {
                GameEditor->CommandList.AddCommand<SetRawMaterial>(&go, static_cast<Material*>(material.GetMaterial()->Clone()));
                GameEditor->CommandList.Redo();
            }
            else
            {
                GameEditor->CommandList.AddCommand<SetRawMaterial>(&go, new Material());
                GameEditor->CommandList.Redo();
            }
        }

        update = false;
        if (state != CustomTextState::None && state == CustomTextState::NoGlobal && material.IsValid())
        {
            IAssetViewer *viewer = GameEditor->GetAssetViewersRegistry().GetViewer(material.GetMaterial()->GetTypeID());
            if (ImGui::TreeNode("Material editor"))
            {
                update = static_cast<MaterialAssetViewer*>(viewer)->MaterialEditor(*material.GetMaterial());
                ImGui::TreePop();
            }

            if (update)
            {
                if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                    lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<MaterialComponent>>(&go);

                static_cast<MaterialAssetViewer*>(viewer)->ApplyChanges(*material.GetMaterial());
            }
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<MaterialComponent>>(&go);
        GameEditor->CommandList.Redo();
    }
}
