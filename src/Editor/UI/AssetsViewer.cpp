#include "Editor/UI/AssetsViewer.h"

#include "Engine.h"
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include "Assets/AssetsManager.h"
#include "Editor/ImGui/ImFileDialog.h"

const std::string AssetsViewer::imageFilter = "Image file (*.png;*.jpg;*.jpeg;*.bmp;*.tga;*.gif){.png,.jpg,.jpeg,.bmp,.tga,.gif},";

AssetsViewer::AssetsViewer()
{
    Assimp::Importer importer;
    modelFilter += "Model file (";
    std::string tempExt;
    importer.GetExtensionList(tempExt);
    modelFilter += tempExt;
    modelFilter += "){";
    std::replace(tempExt.begin(), tempExt.end(), ';', ',');
    tempExt.erase(std::remove(tempExt.begin(), tempExt.end(), '*'), tempExt.end());
    modelFilter += tempExt;
    modelFilter += "},";
}

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
            ImGui::OpenPopup("Import asset window");

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

        importAssetModal();
    }
    ImGui::EndDock();
}

void AssetsViewer::importAssetModal()
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    bool isOpenTexturePopup = false, isOpenModelPopup = false;
    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Import asset window", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        if (ImGui::Button("Import by file", ImVec2(150, 0)))
            ifd::FileDialog::Instance().Open("AssetOpenDialog", "Open a asset", imageFilter + modelFilter + ".*");

        if (ifd::FileDialog::Instance().IsDone("AssetOpenDialog"))
        {
            if (ifd::FileDialog::Instance().HasResult())
            {
                fs::path res = ifd::FileDialog::Instance().GetResult();
                Assimp::Importer importer;
                int x, y, comp;
                if (stbi_info(res.string().c_str(), &x, &y, &comp))
                {
                    ImGui::CloseCurrentPopup();
                    textureImporter.SetCurrentData(res.string(), x, y, comp);
                    isOpenTexturePopup = true;
                }
                else if (importer.IsExtensionSupported(res.extension().string()))
                {
                    ImGui::CloseCurrentPopup();
                    modelImporter.SetCurrentData(res.string());
                    isOpenModelPopup = true;
                }
            }
            ifd::FileDialog::Instance().Close();
        }

        ImGui::Separator();

        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(150, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }

    if (isOpenTexturePopup)
        ImGui::OpenPopup("Import texture window");
    else if (isOpenModelPopup)
        ImGui::OpenPopup("Import model window");

    modelImporter.ModalWindow("Import model window");
    textureImporter.ModalWindow("Import texture window");
}
