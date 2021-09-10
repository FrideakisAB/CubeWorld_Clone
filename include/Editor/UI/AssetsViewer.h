#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <string>
#include <vector>
#include "Editor/UI/IEditorWindow.h"
#include "Editor/Resources/ModelImporter.h"
#include "Editor/Resources/TextureImporter.h"

class AssetsViewer final : public IEditorWindow {
private:
    ImGuiTextFilter filter;
    std::string selected;
    std::vector<const char*> assetsNames;
    int itemCurrent = 0;

    static const std::string imageFilter;
    std::string modelFilter;

    void importAssetModal();
    ModelImporter modelImporter;
    TextureImporter textureImporter;

public:
    AssetsViewer();

    void Draw() final;
};

#endif
