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
        glBufferData(GL_ARRAY_BUFFER, maxParticleCount * sizeof(RenderData), nullptr, Utils::GetDrawTypeGL(DrawType::Stream));

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
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        isNeedRecreate = false;
    }

    if (isNeedUpdate)
    {
        if (position != 0)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            auto *renderParticles = static_cast<RenderData*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));

            float xDiv = 1.0f / particleSystem->ParticleTexture.Tiles.x;
            float yDiv = 1.0f / particleSystem->ParticleTexture.Tiles.y;
            for (u32 i = 0; i < position; ++i)
            {
                Particle particle = particles[i];

                renderParticles[i].PositionAndSize.x = particle.Position.x;
                renderParticles[i].PositionAndSize.y = particle.Position.y;
                renderParticles[i].PositionAndSize.z = particle.Position.z;
                renderParticles[i].PositionAndSize.w = particle.Size;

                renderParticles[i].Color.x = particle.Color.x;
                renderParticles[i].Color.y = particle.Color.y;
                renderParticles[i].Color.z = particle.Color.z;
                renderParticles[i].Color.w = particle.Color.w;

                renderParticles[i].Rotation = glm::radians(particle.Rotation);
                if(particle.Active)
                {
                    renderParticles[i].SecondData.x = 1.0f;
                    renderParticles[i].SecondData.y = particle.BrightMultiplier;
                }
                else
                {
                    renderParticles[i].SecondData.x = 0.0f;
                    renderParticles[i].SecondData.y = particle.BrightMultiplier;
                }

                if (particleSystem->ParticleTexture.Active)
                {
                    renderParticles[i].SpritePositionAndSize.x = ((u32)particle.Sprite % particleSystem->ParticleTexture.Tiles.x);
                    renderParticles[i].SpritePositionAndSize.y = ((u32)particle.Sprite / particleSystem->ParticleTexture.Tiles.y);
                    renderParticles[i].SpritePositionAndSize.z = xDiv;
                    renderParticles[i].SpritePositionAndSize.w = yDiv;
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
            glBindBuffer(GL_ARRAY_BUFFER, 0);
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
