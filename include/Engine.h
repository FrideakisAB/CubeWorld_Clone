#ifndef ENGINE_H
#define ENGINE_H

#include "Render/Lighting.h"

class RenderSystem;
class AssetsManager;

class Engine {
private:
    RenderSystem *renderSystem;
    AssetsManager *assetsManager;
    Lighting lighting;

public:
    Engine();
    ~Engine();

    [[nodiscard]] inline AssetsManager& GetAssetsManager() const noexcept { return *assetsManager; }
    [[nodiscard]] inline RenderSystem& GetRenderSystem() const noexcept { return *renderSystem; }
    [[nodiscard]] inline Lighting& GetLighting() noexcept { return lighting; }

    void Update();
};

extern Engine* GameEngine;

#endif
