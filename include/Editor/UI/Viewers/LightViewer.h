#ifndef LIGHTVIEWER_H
#define LIGHTVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class LightViewer final : public IViewer {
public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
