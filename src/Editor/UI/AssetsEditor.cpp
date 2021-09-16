#include "Editor/UI/AssetsEditor.h"

#include "Engine.h"
#include "Editor/Editor.h"

void AssetsEditor::Draw()
{
    if (ImGui::BeginDock("Assets editor", &Active))
    {
        if (!GameEditor->SelectedAsset.empty())
        {
            ImGui::Text("Asset: %s", GameEditor->SelectedAsset.c_str());
            auto *asset = GameEngine->GetAssetsManager().GetAsset<IAsset>(GameEditor->SelectedAsset);
            if (auto *viewer = AssetViewersRegistry.GetViewer(asset->GetTypeID()))
                viewer->OnEditorUI(*asset);
        }
    }
    ImGui::EndDock();
}
