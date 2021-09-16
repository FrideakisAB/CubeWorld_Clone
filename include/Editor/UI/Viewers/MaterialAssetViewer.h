#ifndef MATERIALASSETVIEWER_H
#define MATERIALASSETVIEWER_H

#include "Platform.h"
#include "Utils/ShaderUtils.h"
#include "Editor/UI/Viewers/IAssetViewer.h"

struct Material;

class MaterialAssetViewer final : public IAssetViewer {
private:
    u64 lastCommandId{};
    std::string shaderStr;
    std::string paramName;
    std::string textureAsset;
    Utils::ShaderParamValue paramValue;

public:
    void OnEditorUI(IAsset &asset) final;

    bool MaterialEditor(Material &material);
    void ApplyChanges(Material &material);
};

#endif
