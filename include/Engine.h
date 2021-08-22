#ifndef ENGINE_H
#define ENGINE_H

#include <random>
#include "Render/Lighting.h"

class GameScene;
class RenderSystem;
class AssetsManager;

class Engine {
private:
    GameScene *scene;
    RenderSystem *renderSystem;
    AssetsManager *assetsManager;
    std::mt19937* randomEngine;
    Lighting lighting;

public:
    Engine();
    ~Engine();

    [[nodiscard]] inline AssetsManager &GetAssetsManager() const noexcept { return *assetsManager; }
    [[nodiscard]] inline RenderSystem &GetRenderSystem() const noexcept { return *renderSystem; }
    [[nodiscard]] inline Lighting &GetLighting() noexcept { return lighting; }
    [[nodiscard]] inline std::mt19937 &GetRandomEngine() const noexcept{ return *randomEngine; }
    [[nodiscard]] inline GameScene &GetGameScene() const noexcept{ return *scene; }

    void Update();
};

extern Engine *GameEngine;

#endif
