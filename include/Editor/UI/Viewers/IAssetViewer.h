#ifndef IASSETVIEWER_H
#define IASSETVIEWER_H

#include "imgui.h"
#include "Assets/IAsset.h"

class IAssetViewer {
public:
    virtual ~IAssetViewer() = default;

    virtual void OnEditorUI(IAsset &asset) = 0;
};

#endif
