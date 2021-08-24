#include "Editor/Editor.h"

#include <GLFW/glfw3.h>
#include "Components/Transform.h"
#include "Editor/UI/Viewers/TransformViewer.h"

Editor *GameEditor = nullptr;

Editor::Editor()
{
    Menu.RegisterEntry(&fileMenu);
    Menu.RegisterEntry(&editorMenu);
    Menu.RegisterEntry(sceneViewer.GetMenuEntry());
    Menu.RegisterEntry(&windowsMenu);

    windowsMenu.Windows["Scene viewer"] = &sceneViewer;
    windowsMenu.Windows["Scene editor"] = &sceneEditor;

    sceneEditor.ViewersRegistry.RegisterViewer<TransformViewer, Transform>();
}

Editor::~Editor()
{
    CommandList.InvalidateAll();
    CacheSystem.SafeClean();
}

void Editor::DrawWindows()
{
    sceneViewer.Draw();
    sceneEditor.Draw();
}

Editor::EditorMenu::EditorMenu()
    : IMenuEntry("Edit")
{}

void Editor::EditorMenu::Draw()
{
    if(RegisterItem("Redo", "Ctrl+Y", false, GameEditor->CommandList.IsRedoActive()))
        GameEditor->CommandList.Redo();
    if(RegisterItem("Undo", "Ctrl+Z", false, GameEditor->CommandList.IsUndoActive()))
        GameEditor->CommandList.Undo();
}

Editor::FileMenu::FileMenu()
    : IMenuEntry("File")
{}

void Editor::FileMenu::Draw()
{
    if(RegisterItem("Exit"))
        glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
}

Editor::WindowsMenu::WindowsMenu()
: IMenuEntry("Windows")
{}

void Editor::WindowsMenu::Draw()
{
    for (auto &&[name, window] : Windows)
        if (RegisterItem(name))
            window->Active = !window->Active;
}
