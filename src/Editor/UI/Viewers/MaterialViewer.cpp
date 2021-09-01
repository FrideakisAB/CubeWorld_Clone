#include "Editor/UI/Viewers/MaterialViewer.h"

#include "Editor/Editor.h"
#include "Systems/RenderSystem.h"
#include "Editor/ImGui/imgui_custom.h"
#include "Components/MaterialComponent.h"
#include "Editor/Commands/ViewersCommands.h"

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

        if (ImGui::TextHandleButton("Material", context.c_str(), "Material", state, 16))
            ImGui::OpenPopup("AssetSelector");

        std::string asset;
        if (ImGui::BeginPopup("AssetSelector"))
        {
            bool isHave = false;
            for (const auto &[name, handle] : GameEngine->GetAssetsManager())
            {
                if (auto *materialPtr = dynamic_cast<Material*>(handle.get()))
                {
                    isHave = true;
                    if (state != CustomTextState::Global && (state == CustomTextState::None || name != context))
                        ImGui::Text(name.c_str());
                    else
                        ImGui::TextColored(ImVec4(255, 230, 5,255), name.c_str());

                    if (ImGui::IsItemClicked())
                    {
                        asset = name;
                        if (state == CustomTextState::None || name != context)
                            update = true;
                        ImGui::CloseCurrentPopup();
                    }
                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0,0,190,255), "(Material)");
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
                ImGui::Text("Not have material assets");

            ImGui::EndPopup();
        }

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
        if (state != CustomTextState::None && state == CustomTextState::NoGlobal)
        {
            if (ImGui::TreeNode("Material editor"))
            {
                std::vector<const char*> shadersName;
                int itemCurrent;
                for (const auto &[name, shader] : GameEngine->GetRenderSystem().GetShaders())
                {
                    if (name == material.GetMaterial()->Shader)
                        itemCurrent = shadersName.size();
                    shadersName.push_back(name.c_str());
                }

                ImGui::Combo("Shader", &itemCurrent, &shadersName[0], shadersName.size());
                if (material.GetMaterial()->Shader != shadersName[itemCurrent])
                    update = true;

                ImGui::TreePop();
            }
        }

        if (update)
        {
            //TODO: material history
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<MaterialComponent>>(&go);
        GameEditor->CommandList.Redo();
    }
}
