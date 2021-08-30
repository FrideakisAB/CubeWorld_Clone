#ifndef TRANSFORMVIEWER_H
#define TRANSFORMVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class TransformViewer final : public IViewer {
private:
    u64 lastCommandId = 0;

public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
