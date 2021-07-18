#include <Render/Render.h>

#include "ECS/ECS.h"
#include "Systems/RenderSystem.h"

void ReleaseHandle(const std::function<void()> &deleter)
{
    ECS::ECS_Engine->GetSystemManager()->GetSystem<RenderSystem>()->ReleaseHandle(deleter);
}

