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

        const char *context;
        CustomTextState state;

        if (!mesh.IsValid())
        {
            if (mesh.GetAsset())
            {
                context = mesh.GetAsset()->GetName().c_str();
                if (mesh.GetAsset()->GetName().empty())
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
            context = mesh.GetAsset()->GetName().c_str();
            state = CustomTextState::Global;
            if (mesh.GetAsset()->GetName().empty())
            {
                context = "(custom)";
                state = CustomTextState::NoGlobal;
            }
        }

        if (ImGui::TextHandleButton("Mesh", context, state, 16))
            ImGui::OpenPopup("AssetSelector");

        std::string asset;
        if (ImGui::BeginPopup("AssetSelector"))
        {
            bool isHave = false;
            for (const auto &[name, handle] : GameEngine->GetAssetsManager())
            {
                if (auto *meshPtr = dynamic_cast<Mesh*>(handle.get()))
                {
                    isHave = true;
                    if (state != CustomTextState::Global && (state == CustomTextState::None || name != mesh.GetAsset()->GetName()))
                        ImGui::Text(name.c_str());
                    else
                        ImGui::TextColored(ImVec4(255, 230, 5,255), name.c_str());

                    if (ImGui::IsItemClicked())
                    {
                        asset = name;
                        if (state == CustomTextState::None || name != mesh.GetAsset()->GetName())
                            update = true;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0,0,190,255), "(Mesh)");
                }
            }

            if (state != CustomTextState::None)
                ImGui::Text("None");
            else
                ImGui::TextColored(ImVec4(255, 230, 5,255), "None");

            if (ImGui::IsItemClicked())
            {
                asset = "";
                if (state != CustomTextState::None)
                    update = true;
                ImGui::CloseCurrentPopup();
            }
            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0,0,190,255), "(Void)");

            if (!isHave)
                ImGui::Text("Not have mesh assets");

            ImGui::EndPopup();
        }

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
