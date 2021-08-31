#include "Editor/Editor.h"

#include "Components/Components.h"
#include "Editor/UI/Viewers/MeshViewer.h"
#include "Editor/UI/Viewers/LightViewer.h"
#include "Editor/UI/Viewers/CameraViewer.h"
#include "Editor/UI/Viewers/ParticleViewer.h"
#include "Editor/UI/Viewers/TransformViewer.h"
#include <GLFW/glfw3.h>

Editor *GameEditor = nullptr;

Editor::Editor()
{
    Menu.RegisterEntry(&fileMenu);
    Menu.RegisterEntry(&editorMenu);
    Menu.RegisterEntry(sceneViewer.GetMenuEntry());
    Menu.RegisterEntry(&windowsMenu);

    windowsMenu.Windows["Scene viewer"] = &sceneViewer;
    windowsMenu.Windows["Scene editor"] = &sceneEditor;

    sceneEditor.ViewersRegistry.RegisterViewer<CameraViewer, Camera>();
    sceneEditor.ViewersRegistry.RegisterViewer<LightViewer, LightSource>();
    sceneEditor.ViewersRegistry.RegisterViewer<MeshViewer, MeshComponent>();
    sceneEditor.ViewersRegistry.RegisterViewer<TransformViewer, Transform>();
    sceneEditor.ViewersRegistry.RegisterViewer<ParticleViewer, ParticleSystem>();
}

Editor::~Editor()
{
    CommandList.InvalidateAll();
    CacheSystem.SafeClean();
}

void Editor::DrawWindows()
{
    CommandList.Update();
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
        RegisterItemSelectable(name, "", &window->Active);
}
