#include "Editor/UI/AssetsViewer.h"

#include "Engine.h"
#include "Assets/AssetsManager.h"

void AssetsViewer::Draw()
{
    AssetsFactory &assetsFactory = GameEngine->GetAssetsManager().GetAssetsFactory();
    if (assetsNames.empty())
    {
        assetsNames.reserve(assetsFactory.astSetRegistry.size() + 1);
        assetsNames.push_back("None");
        for (const auto &value : assetsFactory.astSetRegistry)
            assetsNames.push_back(value.first.c_str());
    }

    if (ImGui::BeginDock("Assets viewer", &Active))
    {
        if (ImGui::Button("Import asset"))
            ImGui::OpenPopup("ImportPopup");

        ImGui::SameLine();
        ImGui::SetNextItemWidth(150.0f);
        ImGui::Combo("Type", &itemCurrent, &assetsNames[0], assetsNames.size());

        ImGui::SameLine();
        filter.Draw("Filter", 100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto &assetSet : GameEngine->GetAssetsManager())
        {
            bool nameFilterFlag = !filter.IsActive() || filter.PassFilter(assetSet.first.c_str(), assetSet.first.c_str() + assetSet.first.size());
            bool typeFilterFlag = itemCurrent == 0 || assetsFactory.astSetRegistry[assetsNames[itemCurrent]].second == assetSet.second->GetTypeID();
            if (nameFilterFlag && typeFilterFlag)
            {
                bool isSelected = assetSet.first == selected;
                if (ImGui::Selectable(assetSet.first.c_str(), isSelected))
                    selected = assetSet.first;
                if (ImGui::BeginDragDropSource())
                {
                    std::string_view asset = assetSet.first;
                    ImGui::SetDragDropPayload("ASSET_", &asset, sizeof(std::string_view));
                    ImGui::Text("Asset: %s", assetSet.first.c_str());
                    ImGui::EndDragDropSource();
                }
            }
        }
        ImGui::EndChild();

        ImVec2 center = ImGui::GetIO().DisplaySize;
        center.x /= 2.0f;
        center.y /= 2.0f;
        ImGui::SetNextWindowPos(center, ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));

        if (ImGui::BeginPopupModal("ImportPopup", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
        {
            ImGui::Text("Import asset window");
            ImGui::Separator();

            if (ImGui::Button("Import", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::SetItemDefaultFocus();
            ImGui::SameLine();
            if (ImGui::Button("Cancel", ImVec2(120, 0))) { ImGui::CloseCurrentPopup(); }
            ImGui::EndPopup();
        }
    }
    ImGui::EndDock();
}
