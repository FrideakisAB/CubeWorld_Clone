#include "Render/ParticleRender.h"

#include "Components/ParticleSystem.h"
#include "Render/GLUtils.h"

const u8 VertexAttribCount = 5;
const Attrib ParticleRender::VertexAttrib[5] = {
        {4, sizeof(ParticleRender::RenderData), 0, ValueType::Float, false},
        {4, sizeof(ParticleRender::RenderData), (u32)offsetof(ParticleRender::RenderData, Color), ValueType::Float, false},
        {2, sizeof(ParticleRender::RenderData), (u32)offsetof(ParticleRender::RenderData, SecondData), ValueType::Float, false},
        {4, sizeof(ParticleRender::RenderData), (u32)offsetof(ParticleRender::RenderData, SpritePositionAndSize), ValueType::Float, false},
        {1, sizeof(ParticleRender::RenderData), (u32)offsetof(ParticleRender::RenderData, Rotation), ValueType::Float, false}
};

ParticleRender::~ParticleRender()
{
    auto deleter = [=](){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
    };
    ReleaseHandle(deleter);
}

void ParticleRender::SubmitData(DrawData &drawData)
{
    if (isNeedRecreate)
    {
        if (VAO != 0)
            glDeleteVertexArrays(1, &VAO);
        if (VBO != 0)
            glDeleteBuffers(1, &VBO);

        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, maxParticleCount * sizeof(RenderData), nullptr, Utils::GetDrawTypeGL(DrawType::Dynamic));

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, sizeof(RenderData), nullptr);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(RenderData), (void*)offsetof(ParticleRender::RenderData, Color));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(RenderData), (void*)offsetof(ParticleRender::RenderData, SecondData));
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(RenderData), (void*)offsetof(ParticleRender::RenderData, SpritePositionAndSize));
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, sizeof(RenderData), (void*)offsetof(ParticleRender::RenderData, Rotation));

        glBindVertexArray(0);

        isNeedRecreate = false;
    }

    if (isNeedUpdate)
    {
        if (position != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            auto *renderParticles = static_cast<RenderData*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (u32 i = 0; i < position; ++i)
            {
                renderParticles[i].PositionAndSize = glm::vec4(particles[i].Position, particles[i].Size);
                renderParticles[i].Color = particles[i].Color;
                renderParticles[i].Rotation = particles[i].Rotation;
                if(particles[i].Active)
                    renderParticles[i].SecondData = glm::vec2(1.0f, particles[i].BrightMultiplier);
                else
                    renderParticles[i].SecondData = glm::vec2(0.0f, particles[i].BrightMultiplier);

                if (particleSystem->ParticleTexture.Active)
                {
                    renderParticles[i].SpritePositionAndSize.x = ((u32)particles[i].Sprite % particleSystem->ParticleTexture.Tiles.y);
                    renderParticles[i].SpritePositionAndSize.y = ((u32)particles[i].Sprite / particleSystem->ParticleTexture.Tiles.x);
                    renderParticles[i].SpritePositionAndSize.z = 1.0f / particleSystem->ParticleTexture.Tiles.x;
                    renderParticles[i].SpritePositionAndSize.w = 1.0f / particleSystem->ParticleTexture.Tiles.y;
                }
                else
                {
                    renderParticles[i].SpritePositionAndSize.x = 0.0f;
                    renderParticles[i].SpritePositionAndSize.y = 0.0f;
                    renderParticles[i].SpritePositionAndSize.z = 1.0f;
                    renderParticles[i].SpritePositionAndSize.w = 1.0f;
                }
            }
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        isNeedUpdate = false;
    }

    drawData.VAO = VAO;
    drawData.Mode = DrawMode::Points;
    drawData.Primitive = Primitive::Points;
    drawData.Count = position;
    drawData.Arrays = true;
}

void ParticleRender::Initialize(ParticleSystem *particleSystem)
{
    this->particleSystem = particleSystem;
}

void ParticleRender::UpdateData(u32 newMaxParticlesCount, Particle *newParticles, u32 newPosition)
{
    if (newMaxParticlesCount != maxParticleCount)
        isNeedRecreate = true;

    isNeedUpdate = true;

    maxParticleCount = newMaxParticlesCount;
    particles = newParticles;
    position = newPosition;
}
