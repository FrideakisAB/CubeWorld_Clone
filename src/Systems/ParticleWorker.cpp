#include "Systems/ParticleWorker.h"

#include "Components/Transform.h"
#include "Components/MaterialComponent.h"

void ParticleWorker::PreUpdate()
{
    auto *CM = ECS::ECS_Engine->GetComponentManager();
    auto *EM = ECS::ECS_Engine->GetEntityManager();

    for (auto &ps : CM->GetIterator<ParticleSystem>())
    {
        auto *entity = EM->GetEntity(ps.GetOwner());
        if (entity != nullptr && entity->IsActive() && ps.IsActive())
        {
            auto *transform = entity->GetComponent<Transform>();
            if (transform != nullptr && transform->IsActive())
                particleTasks.push_back(&ps);
        }
    }
}

void ParticleWorker::Update()
{
    for (ParticleSystem *ps : particleTasks)
        ps->Update();
}

void ParticleWorker::PostUpdate()
{
    size_t oldSize = particleTasks.size();
    particleTasks.clear();
    particleTasks.reserve(oldSize);
}
