#include "Editor/Editor.h"

Editor *GameEditor = nullptr;

Editor::Editor()
{
    Menu.RegisterEntry(&menu);
    Menu.RegisterEntry(sceneViewer.GetMenuEntry());
}

Editor::~Editor()
{
    CommandList.InvalidateAll();
    CacheSystem.SafeClean();
}

void Editor::DrawWindows()
{
    sceneViewer.Draw();
}

Editor::EditorMenu::EditorMenu()
    : IMenuEntry("Edit")
{}

void Editor::EditorMenu::Draw()
{
    if(RegisterItem("Redo", "Ctrl+Y"))
        GameEditor->CommandList.Redo();
    if(RegisterItem("Undo", "Ctrl+Z"))
        GameEditor->CommandList.Undo();
}
