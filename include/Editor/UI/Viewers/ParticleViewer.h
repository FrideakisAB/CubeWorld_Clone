#ifndef PARTICLEVIEWER_H
#define PARTICLEVIEWER_H

#include "Editor/UI/Viewers/IViewer.h"

class ParticleViewer final : public IViewer {
private:
    u64 lastCommandIds[14]{};

public:
    void OnEditorUI(GameObject &go, ECS::IComponent &cmp) final;
};

#endif
