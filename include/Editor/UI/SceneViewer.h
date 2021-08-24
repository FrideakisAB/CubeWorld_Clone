#ifndef SCENEVIEWER_H
#define SCENEVIEWER_H

#include <string>
#include "GameObject.h"
#include "Editor/UI/IMenuEntry.h"
#include "Editor/UI/IEditorWindow.h"

class SceneViewer final : public IEditorWindow {
private:
    class SceneMenu final : public IMenuEntry {
    public:
        ECS::EntityId targetObjectId = ECS::INVALID_ENTITY_ID;

        SceneMenu();
        void Draw() override;
    };

    ECS::EntityId deletedObjectId = ECS::INVALID_ENTITY_ID;
    ECS::EntityId parentObjectId = ECS::INVALID_ENTITY_ID;
    ECS::EntityId targetObjectId = ECS::INVALID_ENTITY_ID;
    SceneMenu menu;

    void recursiveTreeDraw(const std::string &name, GameObject *gameObject);

public:
    void Draw() final;
    IMenuEntry *GetMenuEntry() final { return &menu; }
};

#endif
