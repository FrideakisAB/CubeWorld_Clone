#ifndef GAMEWINDOW_H
#define GAMEWINDOW_H

#include "Editor/CacheSystem.h"
#include "Editor/UI/IEditorWindow.h"

class GameWindow final : public IEditorWindow {
public:
    enum class State {
        Run = 0,
        Paused,
        Stop
    };

private:
    State state = State::Stop;
    CacheEntry cache;
    f32 oldScaled = 1.0f;

public:
    ~GameWindow() final;

    void Draw() final;

    [[nodiscard]] State GetCurrentState() const noexcept { return state; }
};

#endif
