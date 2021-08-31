#ifndef MESHVIEWER_H
#define MESHVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class MeshViewer final : public IViewer {
public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
