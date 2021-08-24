#ifndef IVIEWER_H
#define IVIEWER_H

#include "imgui.h"
#include "ECS/ECS.h"
#include "GameObject.h"

class IViewer {
public:
    virtual ~IViewer() = default;

    virtual void OnEditorUI(GameObject &go, ECS::IComponent &cmp) = 0;
};

#endif
