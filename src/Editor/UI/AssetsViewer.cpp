#include "Editor/UI/AssetsViewer.h"

#include "Engine.h"
#include "Assets/AssetsManager.h"

void AssetsViewer::Draw()
{
    if (ImGui::BeginDock("Assets viewer", &Active))
    {
        bool isImport = ImGui::Button("Import asset");
        ImGui::SameLine();
        filter.Draw("Filter", -100.0f);

        ImGui::Separator();
        ImGui::BeginChild("scrolling", ImVec2(0,0), false, ImGuiWindowFlags_HorizontalScrollbar);
        for (const auto &assetSet : GameEngine->GetAssetsManager())
        {
            if (!filter.IsActive() || filter.PassFilter(assetSet.first.c_str(), assetSet.first.c_str() + assetSet.first.size()))
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
    }
    ImGui::EndDock();
}
