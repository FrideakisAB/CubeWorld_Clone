#ifndef ASSETSEDITOR_H
#define ASSETSEDITOR_H

#include "Editor/UI/IEditorWindow.h"
#include "Editor/UI/Viewers/AssetViewersRegister.h"

class AssetsEditor final : public IEditorWindow {
public:
    void Draw() final;

    AssetViewersRegister AssetViewersRegistry;
};

#endif
