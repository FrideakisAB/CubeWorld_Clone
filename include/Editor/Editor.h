#ifndef EDITOR_H
#define EDITOR_H

#include "ECS/ECS.h"
#include "Editor/UI/MenuBar.h"
#include "Editor/CacheSystem.h"
#include "Editor/UI/LogViewer.h"
#include "Editor/UI/GameWindow.h"
#include "Editor/UI/SceneViewer.h"
#include "Editor/UI/SceneEditor.h"
#include "Editor/UI/AssetsViewer.h"
#include "Editor/UI/AssetsEditor.h"
#include "Editor/UI/EditorViewer.h"
#include "Editor/UI/LightingWindow.h"
#include "Editor/Render/EditorRender.h"
#include "Editor/Commands/CommandList.h"
#include "Editor/Resources/AssetsWriter.h"
#include "Editor/Resources/ConfigManager.h"

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
    private:
        json data = {};
        bool isFirstSet = false;

    public:
        WindowsMenu();
        ~WindowsMenu() final;
        void Draw() override;

        std::map<std::string, IEditorWindow*> Windows;
    };

    LogViewer logViewer;
    AssetsViewer assetsViewer;
    EditorViewer editorViewer;
    SceneViewer sceneViewer;
    SceneEditor sceneEditor;
    AssetsEditor assetsEditor;
    EditorMenu editorMenu;
    FileMenu fileMenu;
    WindowsMenu *windowsMenu;
    GameWindow *gameWindow;
    LightingWindow lightingWindow;
    EditorRender render;
    AssetsWriter assetsWriter;
    ConfigManager configManager;

public:
    Editor();
    ~Editor();

    void DrawWindows();

    [[nodiscard]] EditorRender &GetRender() noexcept { return render; }
    [[nodiscard]] AssetsWriter &GetAssetsWriter() noexcept { return assetsWriter; }
    [[nodiscard]] ViewersRegister &GetViewersRegistry() noexcept { return sceneEditor.ViewersRegistry; }
    [[nodiscard]] AssetViewersRegister &GetAssetViewersRegistry() noexcept { return assetsEditor.AssetViewersRegistry; }
    [[nodiscard]] ConfigManager &GetConfigManager() noexcept { return configManager; }

    bool IsActiveSimulate = false;
    ECS::EntityId Selected = ECS::INVALID_ENTITY_ID;
    std::string SelectedAsset;
    CommandList CommandList;
    CacheSystem CacheSystem;
    MenuBar Menu;
};

extern Editor *GameEditor;

#endif
