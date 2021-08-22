#ifndef EDITOR_H
#define EDITOR_H

#include "ECS/ECS.h"
#include "Editor/UI/MenuBar.h"
#include "Editor/CacheSystem.h"
#include "Editor/UI/SceneViewer.h"
#include "Editor/Commands/CommandList.h"

class Editor {
private:
    class EditorMenu final : public IMenuEntry {
    public:
        EditorMenu();
        void Draw() override;
    };

    SceneViewer sceneViewer;
    EditorMenu menu;

public:
    Editor();
    ~Editor();

    void DrawWindows();

    ECS::EntityId Selected = ECS::INVALID_ENTITY_ID;
    CommandList CommandList;
    CacheSystem CacheSystem;
    MenuBar Menu;
};

extern Editor *GameEditor;

#endif
