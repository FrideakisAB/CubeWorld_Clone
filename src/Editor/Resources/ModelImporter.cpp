#include "Editor/Resources/ModelImporter.h"

#include "imgui.h"
#include "Engine.h"
#include <assimp/scene.h>
#include "Editor/Editor.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Assets/AssetsManager.h"
#include "Editor/ImGui/ImFileDialog.h"

void ModelImporter::SetCurrentData(const fs::path &path)
{
    currentPath = path.string();
    assetName = path.stem().string();

    isImportMaterials = true;
    isImportTextures = true;
    isImportAnimations = true;
    isImportSkeleton = true;
    isImportLights = true;
    isImportCameras = true;
}

void ModelImporter::ModalWindow(const std::string &name)
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        if (isLoadedFinish)
            drawParameters();
        else
            drawProgressBar();

        ImGui::EndPopup();
    }
}

void ModelImporter::processModel()
{
    importState = 0.0f;
    importStateName = "Load raw model";

    Assimp::Importer importer;
    int flags = aiProcess_GenSmoothNormals |
                aiProcess_JoinIdenticalVertices |
                aiProcess_ImproveCacheLocality |
                aiProcess_LimitBoneWeights |
                aiProcess_RemoveRedundantMaterials |
                aiProcess_SplitLargeMeshes |
                aiProcess_Triangulate |
                aiProcess_GenUVCoords |
                aiProcess_TransformUVCoords |
                aiProcess_SortByPType |
                aiProcess_FindDegenerates |
                aiProcess_FindInvalidData;
    const aiScene *scene = importer.ReadFile(currentPath, flags);

    importState += 1.0f / 9.0f;
    importStateName = "Process hierarchy";
}

void ModelImporter::drawParameters()
{
    ImGui::Text("Path: %s", currentPath.c_str());
    ImGui::Separator();

    ImGui::Checkbox("Import material", &isImportMaterials);
    ImGui::Checkbox("Import textures", &isImportTextures);
    ImGui::Checkbox("Import animations", &isImportAnimations);
    ImGui::Checkbox("Import skeleton", &isImportSkeleton);
    ImGui::Checkbox("Import lights", &isImportLights);
    ImGui::Checkbox("Import cameras", &isImportCameras);

    std::string label = "Asset name";
    bool isUniqName = GameEngine->GetAssetsManager().GetAsset(assetName) == nullptr;
    if (!isUniqName)
        label += "(Not uniq)";
    else
        label += "(Uniq)";

    char buffer[256] = {0};
    if (assetName.size() < 256)
        std::copy(assetName.begin(), assetName.end(), buffer);
    ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
    assetName = buffer;
    ImGui::Separator();

    if (ImGui::Button("Apply", ImVec2(150, 0)) && isUniqName)
    {
        //TODO: save and add assets
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    ImGui::SetItemDefaultFocus();
    if (ImGui::Button("Cancel", ImVec2(150, 0)))
        ImGui::CloseCurrentPopup();
}

void ModelImporter::drawProgressBar()
{
    ImGui::ProgressBar(importState);
    ImGui::Text("%s", importStateName.data());
}
