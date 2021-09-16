#include "Editor/UI/AssetsViewer.h"

#include "Engine.h"
#include <stb_image.h>
#include "Editor/Editor.h"
#include "Assets/Prefab.h"
#include <assimp/Importer.hpp>
#include "Assets/AssetsManager.h"
#include "Editor/ImGui/ImCustom.h"
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

    json staticReference = json_utils::TryParse(Utils::FileToString(std::ifstream("data/staticReference.db")));

    for (auto &value : staticReference.items())
    {
        json assetData = json_utils::TryParse(Utils::FileToString(std::ifstream(value.value()[0].get<std::string>())));

        size_t typeId = GameEngine->GetAssetsManager().GetAssetsFactory().GetID(assetData["nameType"].get<std::string>());
        staticAssets.emplace_back(value.key(), typeId);
    }
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
            if (auto It = std::find(staticAssets.begin(), staticAssets.end(), std::pair(assetSet.first, assetSet.second->GetTypeID())); It != staticAssets.end())
                staticAssets.erase(It);

            displayAsset(assetSet.first, assetSet.second->GetTypeID());
        }

        for (const auto &assetName : staticAssets)
            displayAsset(assetName.first, assetName.second);

        if (ImVec2 sz = ImGui::GetContentRegionAvail(); sz.x > 0.0f && sz.y > 0.0f)
        {
            if (ImGui::InvisibleButton("AVBottomButton", sz))
                GameEditor->SelectedAsset = "";

            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_GO"))
                {
                    toPrefab = *((ECS::EntityId*)payload->Data);
                    prefabName = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(toPrefab))->Name;
                    ImGui::OpenPopup("Save prefab window");
                    ImGui::EndDragDropTarget();
                }
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_MATERIAL"))
                {
                    materialName = "Material";
                    toMaterial = *((MaterialComponent**)payload->Data);
                    ImGui::OpenPopup("Save material window");
                    ImGui::EndDragDropTarget();
                }
            }
        }
        saveMaterialModal();
        savePrefabModal();
        ImGui::EndChild();

        importAssetModal();
    }
    ImGui::EndDock();

    if (!deletedAsset.first.empty())
    {
        if (deletedAsset.second != boost::typeindex::type_id<Prefab>().hash_code())
        {
            auto future = GameEditor->GetAssetsWriter().RemoveAsset(deletedAsset.first);
            if (future.valid())
                future.get();

            json staticReference = json_utils::TryParse(Utils::FileToString(std::ifstream("data/staticReference.db")));

            if (staticReference.find(deletedAsset.first) != staticReference.end())
            {
                deleteAsset(staticReference, deletedAsset.first);

                std::ofstream staticReferenceFile("data/staticReference.db", std::ios::trunc);
                staticReferenceFile << staticReference.dump(4);
                staticReferenceFile.close();
            }

            deletedAsset = {};
        }
        else
        {
            auto &assetsManager = GameEngine->GetAssetsManager();
            json staticReference = json_utils::TryParse(Utils::FileToString(std::ifstream("data/staticReference.db")));

            auto assetIt = assetsManager.assets.find(deletedAsset.first);
            if (staticReference.find(deletedAsset.first) != staticReference.end() && (assetIt == assetsManager.assets.end() || !assetIt->second->IsDynamic()))
            {
                auto srIt = staticReference.begin();
                while (srIt != staticReference.end())
                {
                    std::string staticAssetName = srIt.key();
                    if (staticAssetName.find(deletedAsset.first) != std::string::npos && staticAssetName != deletedAsset.first)
                    {
                        deleteAsset(staticReference, staticAssetName);
                        srIt = staticReference.begin();
                    }
                    else
                        ++srIt;
                }
            }
            else
            {
                auto srIt = assetsManager.assets.begin();
                while (srIt != assetsManager.assets.end())
                {
                    std::string staticAssetName = srIt->first;
                    if (staticAssetName.find(deletedAsset.first) != std::string::npos && staticAssetName != deletedAsset.first)
                    {
                        deleteAsset(staticReference, staticAssetName);
                        srIt = assetsManager.assets.begin();
                    }
                    else
                        ++srIt;
                }
            }

            deleteAsset(staticReference, deletedAsset.first);

            deletedAsset = {};

            std::ofstream staticReferenceFile("data/staticReference.db", std::ios::trunc);
            staticReferenceFile << staticReference.dump(4);
            staticReferenceFile.close();
        }
    }
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

void AssetsViewer::displayAsset(const std::string &name, size_t type)
{
    bool nameFilterFlag = !filter.IsActive() || filter.PassFilter(name.c_str(), name.c_str() + name.size());
    bool typeFilterFlag = itemCurrent == 0 || GameEngine->GetAssetsManager().GetAssetsFactory().astSetRegistry[assetsNames[itemCurrent]].second == type;
    if (nameFilterFlag && typeFilterFlag)
    {
        static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth |
                                              ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGuiTreeNodeFlags nodeFlags = baseFlags;

        if (name == GameEditor->SelectedAsset)
            nodeFlags |= ImGuiTreeNodeFlags_Selected;

        ImGui::TreeNodeEx(name.c_str(), nodeFlags);
        if (ImGui::IsItemClicked())
            GameEditor->SelectedAsset = name;
        if (ImGui::BeginDragDropSource())
        {
            std::string_view asset = name;
            ImGui::SetDragDropPayload("ASSET_", &asset, sizeof(std::string_view));
            ImGui::Text("Asset: %s", name.c_str());
            if (type == boost::typeindex::type_id<Texture>().hash_code())
                ImGui::TextureWidget(name, ImVec2(128, 128));
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginPopupContextItem(name.c_str()))
        {
            if (ImGui::MenuItem("Delete"))
                deletedAsset = {name, type};
            ImGui::EndPopup();
        }
    }
}

void AssetsViewer::deleteAsset(json &staticReference, const std::string &asset)
{
    if (GameEditor->SelectedAsset == asset)
        GameEditor->SelectedAsset = "";

    auto future = GameEditor->GetAssetsWriter().RemoveAsset(asset);
    if (future.valid())
        future.get();

    if (!staticReference[asset][0].get<std::string>().empty())
        fs::remove(staticReference[asset][0].get<std::string>());
    if (!staticReference[asset][1].get<std::string>().empty())
        fs::remove(staticReference[asset][1].get<std::string>());

    if (GameEngine->GetAssetsManager().staticResources.find(asset) != GameEngine->GetAssetsManager().staticResources.end())
        GameEngine->GetAssetsManager().staticResources.erase(asset);
    GameEngine->GetAssetsManager().assets.erase(asset);
    staticReference.erase(asset);

    auto It = std::find_if(staticAssets.begin(), staticAssets.end(), [&](const std::pair<std::string, size_t> &val){
        return val.first == asset;
    });
    if (It != staticAssets.end())
        staticAssets.erase(It);
}

void AssetsViewer::savePrefabModal()
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Save prefab window", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        std::string label = "Prefab name";
        bool isUniqName = GameEngine->GetAssetsManager().GetAsset(prefabName) == nullptr;
        if (!isUniqName)
            label += "(Not uniq)";
        else
            label += "(Uniq)";

        char buffer[256] = {0};
        if (prefabName.size() < 256)
            std::copy(prefabName.begin(), prefabName.end(), buffer);
        ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
        prefabName = buffer;

        ImGui::Separator();

        if (ImGui::Button("Save", ImVec2(150, 0)) && isUniqName)
        {
            AssetsHandle prefab = std::make_shared<Prefab>(*static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(toPrefab)));
            GameEngine->GetAssetsManager().AddAsset(prefabName, prefab);
            GameEditor->GetAssetsWriter().AddAsset(prefab);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(150, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}

void AssetsViewer::saveMaterialModal()
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal("Save material window", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        std::string label = "Material name";
        bool isUniqName = GameEngine->GetAssetsManager().GetAsset(materialName) == nullptr;
        if (!isUniqName)
            label += "(Not uniq)";
        else
            label += "(Uniq)";

        char buffer[256] = {0};
        if (materialName.size() < 256)
            std::copy(materialName.begin(), materialName.end(), buffer);
        ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
        materialName = buffer;

        ImGui::Separator();

        if (ImGui::Button("Save", ImVec2(150, 0)) && isUniqName)
        {
            AssetsHandle material = toMaterial->GetMaterialHandle();
            GameEngine->GetAssetsManager().AddAsset(materialName, material);
            GameEditor->GetAssetsWriter().AddAsset(material);
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        ImGui::SetItemDefaultFocus();
        if (ImGui::Button("Cancel", ImVec2(150, 0)))
            ImGui::CloseCurrentPopup();
        ImGui::EndPopup();
    }
}
