#ifndef SCENEEDITOR_H
#define SCENEEDITOR_H

#include "Editor/UI/IEditorWindow.h"
#include "Editor/UI/Viewers/ViewersRegister.h"

class SceneEditor final : public IEditorWindow {
public:
    void Draw() final;

    ViewersRegister ViewersRegistry;
};

#endif
