#ifndef RENDERSYSTEM_H
#define RENDERSYSTEM_H

#include <vector>
#include "ECS/ECS.h"
#include <functional>

class RenderSystem : public ECS::System<RenderSystem> {
private:
    std::vector<std::function<void()>> deletedHandles;

public:
    RenderSystem();
    ~RenderSystem() override;

    void PreUpdate() override;
    void Update() override;
    void PostUpdate() override;

    void ReleaseHandle(const std::function<void()> &deleter)
    {
        deletedHandles.emplace_back(deleter);
    }
};

#endif
