#ifndef PARTICLEWORKER_H
#define PARTICLEWORKER_H

#include "ECS/ECS.h"
#include "Components/ParticleSystem.h"

class ParticleWorker final : public ECS::System<ParticleWorker> {
private:
    std::vector<ParticleSystem*> particleTasks;

public:
    ~ParticleWorker() final = default;

    void PreUpdate() final;
    void Update() final;
    void PostUpdate() final;
};

#endif
