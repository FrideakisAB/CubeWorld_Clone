#ifndef MATERIALVIEWER_H
#define MATERIALVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class MaterialViewer final : public IViewer {
private:
    u64 lastCommandId{};

public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
