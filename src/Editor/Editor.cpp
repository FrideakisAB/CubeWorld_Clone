#include "Editor/Editor.h"

#include "Components/Components.h"
#include "Editor/UI/Viewers/MeshViewer.h"
#include "Editor/UI/Viewers/LightViewer.h"
#include "Editor/UI/Viewers/CameraViewer.h"
#include "Editor/UI/Viewers/MaterialViewer.h"
#include "Editor/UI/Viewers/ParticleViewer.h"
#include "Editor/UI/Viewers/TransformViewer.h"
#include "Editor/UI/Viewers/MaterialAssetViewer.h"
#include <GLFW/glfw3.h>

Editor *GameEditor = nullptr;

Editor::Editor()
{
    GameEditor = this;

    gameWindow = new GameWindow();
    windowsMenu = new WindowsMenu();

    Menu.RegisterEntry(&fileMenu);
    Menu.RegisterEntry(&editorMenu);
    Menu.RegisterEntry(sceneViewer.GetMenuEntry());
    Menu.RegisterEntry(windowsMenu);

    windowsMenu->Windows["Scene viewer"] = &sceneViewer;
    windowsMenu->Windows["Scene editor"] = &sceneEditor;
    windowsMenu->Windows["Assets editor"] = &assetsEditor;
    windowsMenu->Windows["Log viewer"] = &logViewer;
    windowsMenu->Windows["Editor viewer"] = &editorViewer;
    windowsMenu->Windows["Assets viewer"] = &assetsViewer;
    windowsMenu->Windows["Game window"] = gameWindow;
    windowsMenu->Windows["Lighting window"] = &lightingWindow;

    sceneEditor.ViewersRegistry.RegisterViewer<CameraViewer, Camera>();
    sceneEditor.ViewersRegistry.RegisterViewer<LightViewer, LightSource>();
    sceneEditor.ViewersRegistry.RegisterViewer<MeshViewer, MeshComponent>();
    sceneEditor.ViewersRegistry.RegisterViewer<TransformViewer, Transform>();
    sceneEditor.ViewersRegistry.RegisterViewer<ParticleViewer, ParticleSystem>();
    sceneEditor.ViewersRegistry.RegisterViewer<MaterialViewer, MaterialComponent>();

    assetsEditor.AssetViewersRegistry.RegisterViewer<MaterialAssetViewer, Material>();
}

Editor::~Editor()
{
    delete windowsMenu;
    delete gameWindow;
    CommandList.InvalidateAll();
    CacheSystem.SafeClean();
}

void Editor::DrawWindows()
{
    if (!IsActiveSimulate)
        ECS::ECS_Engine->UpdateWithoutSystems();
    CommandList.Update();
    assetsViewer.Draw();
    sceneViewer.Draw();
    sceneEditor.Draw();
    assetsEditor.Draw();
    logViewer.Draw();
    editorViewer.Draw();
    lightingWindow.Draw();
    logViewer.StartCapture();
    gameWindow->Draw();
    configManager.DrawSettings();
    logViewer.EndCapture();
}

Editor::EditorMenu::EditorMenu()
    : IMenuEntry("Edit")
{}

void Editor::EditorMenu::Draw()
{
    if (RegisterItem("Redo", "Ctrl+Y", false, GameEditor->CommandList.IsRedoActive()))
        GameEditor->CommandList.Redo();
    if (RegisterItem("Undo", "Ctrl+Z", false, GameEditor->CommandList.IsUndoActive()))
        GameEditor->CommandList.Undo();
}

Editor::FileMenu::FileMenu()
    : IMenuEntry("File")
{}

void Editor::FileMenu::Draw()
{
    if (RegisterItem("Settings"))
        GameEditor->GetConfigManager().OpenWindow();

    if (RegisterItem("Exit"))
        glfwSetWindowShouldClose(glfwGetCurrentContext(), true);
}

Editor::WindowsMenu::WindowsMenu()
    : IMenuEntry("Windows")
{
    data = GameEditor->GetConfigManager().GetConfig("Windows active");
}

Editor::WindowsMenu::~WindowsMenu()
{
    data = {};
    for (auto &&[name, window] : Windows)
        data[name] = window->Active;

    GameEditor->GetConfigManager().SetConfig("Windows active", data);
}

void Editor::WindowsMenu::Draw()
{
    if (isFirstSet)
    {
        for (auto &&[name, window]: Windows)
            RegisterItemSelectable(name, "", &window->Active);
    }
    else
    {
        isFirstSet = true;
        if (!data.empty())
        {
            for (auto &&[name, window]: Windows)
            {
                if (auto It = data.find(name); It != data.end())
                    window->Active = It.value().get<bool>();
            }
        }
    }
}
