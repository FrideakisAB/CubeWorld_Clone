#ifndef ENGINE_H
#define ENGINE_H

class RenderSystem;
class AssetsManager;

class Engine {
private:
    RenderSystem *renderSystem;
    AssetsManager *assetsManager;

public:
    Engine();
    ~Engine();

    [[nodiscard]] inline AssetsManager& GetAssetsManager() const noexcept { return *assetsManager; }
    [[nodiscard]] inline RenderSystem& GetRenderSystem() const noexcept { return *renderSystem; }

    void Update();
};

extern Engine* GameEngine;

#endif
