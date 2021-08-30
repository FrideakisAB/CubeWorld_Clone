#ifndef CAMERAVIEWER_H
#define CAMERAVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class CameraViewer final : public IViewer {
private:
    u64 lastCommandId = 0;

public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
