#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <string>
#include <vector>
#include "Editor/UI/IEditorWindow.h"
#include "Editor/Resources/ModelImporter.h"
#include "Editor/Resources/TextureImporter.h"

class MaterialComponent;

class AssetsViewer final : public IEditorWindow {
private:
    ImGuiTextFilter filter;
    std::vector<const char*> assetsNames;
    std::vector<std::pair<std::string, size_t>> staticAssets;
    int itemCurrent = 0;

    static const std::string imageFilter;
    std::string modelFilter;

    void importAssetModal();
    ModelImporter modelImporter;
    TextureImporter textureImporter;

    void savePrefabModal();
    std::string prefabName;
    ECS::EntityId toPrefab;

    void saveMaterialModal();
    std::string materialName;
    MaterialComponent *toMaterial;

    void displayAsset(const std::string &name, size_t type);
    std::pair<std::string, size_t> deletedAsset;
    void deleteAsset(json &staticReference, const std::string &asset);

public:
    AssetsViewer();

    void Draw() final;
};

#endif
