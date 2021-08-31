#ifndef MATERIALVIEWER_H
#define MATERIALVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class MaterialViewer final : public IViewer {
public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
