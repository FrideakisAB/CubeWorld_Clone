#ifndef EDITOR_H
#define EDITOR_H

#include "ECS/ECS.h"
#include "Editor/UI/MenuBar.h"
#include "Editor/CacheSystem.h"
#include "Editor/UI/LogViewer.h"
#include "Editor/UI/SceneViewer.h"
#include "Editor/UI/SceneEditor.h"
#include "Editor/Commands/CommandList.h"

class Editor {
private:
    class EditorMenu final : public IMenuEntry {
    public:
        EditorMenu();
        void Draw() override;
    };
    class FileMenu final : public IMenuEntry {
    public:
        FileMenu();
        void Draw() override;
    };
    class WindowsMenu final : public IMenuEntry {
    public:
        WindowsMenu();
        void Draw() override;

        std::map<std::string, IEditorWindow*> Windows;
    };

    LogViewer logViewer;
    SceneViewer sceneViewer;
    SceneEditor sceneEditor;
    EditorMenu editorMenu;
    FileMenu fileMenu;
    WindowsMenu windowsMenu;

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
