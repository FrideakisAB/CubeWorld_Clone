#ifndef LIGHTVIEWER_H
#define LIGHTVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class LightViewer final : public IViewer {
private:
    u64 lastCommandId = 0;

public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
