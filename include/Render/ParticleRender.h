#ifndef PARTICLERENDER_H
#define PARTICLERENDER_H

#include "Platform.h"
#include "Render/Render.h"
#include "Assets/AssetsManager.h"

struct Particle;
class ParticleSystem;

class ParticleRender : public DrawObject {
public:
    struct RenderData {
        glm::vec4 PositionAndSize = glm::vec4(0.0f);
        glm::vec4 Color = glm::vec4(1.0f);
        // X - active state (is 1.0), Y - bright multiplier
        glm::vec2 SecondData = glm::vec2(0.0f);
        glm::vec4 SpritePositionAndSize = glm::vec4(0.0f);
        float Rotation = 0;
    };

    static const u8 VertexAttribCount;
    static const Attrib VertexAttrib[5];

private:
    ParticleSystem *particleSystem;
    u32 maxParticleCount = 0;
    u32 position = 0;
    u32 VAO = 0, VBO = 0;
    Particle *particles = nullptr;
    bool isNeedRecreate = true;
    bool isNeedUpdate = true;

protected:
    void SubmitData(DrawData &drawData) override;

public:
    ~ParticleRender();

    void Initialize(ParticleSystem *particleSystem);
    void UpdateData(u32 newMaxParticlesCount, Particle *newParticles, u32 newPosition);
};

#endif
