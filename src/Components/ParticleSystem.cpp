#include "Components/ParticleSystem.h"

#include "Utils/Math.h"
#include "Utils/Mathf.h"
#include "Utils/Curve.h"
#include "ECS/util/Timer.h"

ParticleSystem::ParticleSystem()
{
    particles = new Particle[maxParticlesCount];
    releasedParticles = new u32[maxParticlesCount];

    for(u32 i = 0; i < 10; ++i)
    {
        auto iFloat = static_cast<f32>(i) * 0.1f;
        SizeOverLifetime.Points[i] = glm::vec2(iFloat, 1.0f);
        SpeedOverLifetime.Points[i] = glm::vec2(iFloat, 1.0f);

        ForceOverLifetime.XPoints[i] = glm::vec2(iFloat, 0.0f);
        ForceOverLifetime.YPoints[i] = glm::vec2(iFloat, 0.0f);
        ForceOverLifetime.ZPoints[i] = glm::vec2(iFloat, 0.0f);

        SizeBySpeed.Points[i] = glm::vec2(iFloat, 1.0f);
    }
}

ParticleSystem::~ParticleSystem()
{
    Emission.Bursts.clear();

    delete[] releasedParticles;
    delete[] particles;
}

Particle ParticleSystem::genParticle() const
{
    Particle particle{};

    particle.Active = true;
    particle.MaxLifetime = StartLifetime;
    particle.Lifetime = 0.0f;
    particle.Speed = StartSpeed;
    particle.Size = StartSize / 10;
    particle.Rotation = StartRotation;
    particle.Color = StartColor;
    particle.BrightMultiplier = StartBright;
    particle.SpritePosition = glm::uvec2(0);

    switch(Shape)
    {
    default: [[fallthrough]];
    case Emitter::Plane:
        particle.Position = glm::vec3(Mathf::RangeRandom(-1.0f, 1.0f), 0.0f, Mathf::RangeRandom(-1.0f, 1.0f));
        particle.Velocity = glm::vec3(0.0f, 1.0f, 0.0f);
        break;

    case Emitter::Circle:
        {
            f32 x = Mathf::RangeRandom(-1.0f, 1.0f);
            f32 z = sqrt(1.0f - x * x);
            z = Mathf::RangeRandom(-z, z);

            particle.Position = glm::vec3(x, 0.0f, z);
            particle.Velocity = glm::vec3(0.0f, 1.0f, 0.0f);
        }
        break;

    case Emitter::Sphere:
        {
            f32 a1 = Mathf::RangeRandom(0.0f, 360.0f);
            f32 a2 = Mathf::RangeRandom(0.0f, 360.0f);
            f32 sina = sin(a1);
            f32 x = sina * cos(a2);
            f32 y = sina * sin(a2);
            f32 z = cos(a1);

            particle.Position = glm::vec3(x, y, z);
            particle.Velocity = glm::vec3(x, y, z);
        }
        break;

    case Emitter::Cone:
        {
            f32 x = Mathf::RangeRandom(-1.0f, 1.0f);
            f32 z = sqrt(1.0f - x * x);
            z = Mathf::RangeRandom(-z, z);

            glm::vec3 vel = glm::vec3(x * 1.5, 1.0f, z * 1.5) - glm::vec3(x, 0.0f, z);
            vel = glm::normalize(vel);

            particle.Position = glm::vec3(x, 0.0f, z);
            particle.Velocity = glm::vec3(vel.x, vel.y, vel.z);
        }
        break;
    }

    return particle;
}

void ParticleSystem::update()
{
    float deltaTime = ECS::ECS_Engine->GetTimer()->GetDeltaTime();
    if (state == ParticleState::Run)
        activeTime += deltaTime;

    if (activeTime > StartDelay && state == ParticleState::Run && (activeTime < Duration || Duration == 0.0f))
    {
        auto count = static_cast<u32>(deltaTime * (float)Emission.Rate);
        if (count != 0)
        {
            if (Loop && releasePosition > 0)
            {
                for (u32 i = releasePosition; i > 0 && count > 0; --i)
                {
                    Particle part = genParticle();
                    particles[releasedParticles[i - 1]] = part;
                    --count;
                    --releasePosition;
                }
            }

            //TODO: apply Loop in this behaviour
            u32 launchCount = count;
            if (launchCount + position > maxParticlesCount)
                launchCount = maxParticlesCount - position;

            for (u32 i = 0; i < launchCount; ++i)
            {
                Particle part = genParticle();
                particles[position++] = part;
            }
        }

        for (auto& Burst : Emission.Bursts)
        {
            if (Burst.Time <= deltaTime && !Burst.IsMake)
            {
                u32 particleCount = Math::RangeRandom(Burst.MinCount, Burst.MaxCount);
                if (particleCount + position > maxParticlesCount)
                    particleCount = maxParticlesCount - position;

                for (u32 i = 0; i < particleCount; ++i)
                {
                    Particle part = genParticle();
                    particles[position++] = part;
                }

                Burst.IsMake = true;
            }
        }

        u32 updateCount = 0;
        for (u32 i = 0; i < position; ++i)
        {
            if (!particles[i].Active)
                continue;

            particles[i].Lifetime += deltaTime;
            if (particles[i].Lifetime > particles[i].MaxLifetime)
            {
                if(Loop)
                    releasedParticles[releasePosition++] = i;

                particles[i].Active = false;
            }

            f32 particleLifetimeAspect = particles[i].Lifetime / particles[i].MaxLifetime;

            if (ColorOverLifetime.Active)
                particles[i].Color = ColorOverLifetime.Gradient.GetColorAt(particleLifetimeAspect);
            else if (ColorBySpeed.Active)
            {
                if (particles[i].Speed >= ColorBySpeed.MinSpeed && particles[i].Speed <= ColorBySpeed.MaxSpeed)
                {
                    if (ColorBySpeed.MaxSpeed != ColorBySpeed.MinSpeed)
                        particles[i].Color = ColorBySpeed.Gradient.GetColorAt((particles[i].Speed - ColorBySpeed.MinSpeed) / (ColorBySpeed.MaxSpeed - ColorBySpeed.MinSpeed));
                    else
                        particles[i].Color = ColorOverLifetime.Gradient.GetColorAt(0.0f);
                }
            }

            if (SpeedOverLifetime.Active)
                particles[i].Speed = StartSpeed * Curve::CurveValue(particleLifetimeAspect, 10, SpeedOverLifetime.Points);

            particles[i].Position += particles[i].Velocity * particles[i].Speed * deltaTime;

            if (ForceOverLifetime.Active)
                particles[i].Position += glm::vec3(ForceOverLifetime.BaseForce * glm::vec3(
                        Curve::CurveValue(particleLifetimeAspect, 10, ForceOverLifetime.XPoints),
                        Curve::CurveValue(particleLifetimeAspect, 10, ForceOverLifetime.YPoints),
                        Curve::CurveValue(particleLifetimeAspect, 10, ForceOverLifetime.ZPoints)));

            if (RotationBySpeed.Active)
            {
                if (particles[i].Speed >= RotationBySpeed.MinSpeed && particles[i].Speed <= RotationBySpeed.MaxSpeed)
                    particles[i].Rotation += RotationBySpeed.BaseSpeed * deltaTime;
            }

            if (RotationOverLifetime.Active)
                particles[i].Rotation += RotationOverLifetime.Speed * deltaTime;

            if (SizeOverLifetime.Active)
                particles[i].Size = (StartSize / 10) * Curve::CurveValue(particleLifetimeAspect, 10, SizeOverLifetime.Points);
            else if (SizeBySpeed.Active && SizeBySpeed.MinSpeed != SizeBySpeed.MaxSpeed)
            {
                particles[i].Size = SizeBySpeed.BaseSize *
                    Curve::CurveValue((particles[i].Speed - SizeBySpeed.MinSpeed) / (SizeBySpeed.MaxSpeed - SizeBySpeed.MinSpeed), 10, SizeBySpeed.Points);
            }

            if (BrightOverLifetime.Active)
                particles[i].BrightMultiplier = BrightOverLifetime.BaseBright * Curve::CurveValue(particleLifetimeAspect, 10, BrightOverLifetime.Points);
            else if (BrightBySpeed.Active && BrightBySpeed.MinSpeed != BrightBySpeed.MaxSpeed)
            {
                particles[i].BrightMultiplier = BrightBySpeed.BaseBright *
                    Curve::CurveValue((particles[i].Speed - BrightBySpeed.MinSpeed) / (BrightBySpeed.MaxSpeed - BrightBySpeed.MinSpeed), 10, BrightBySpeed.Points);
            }

            ++updateCount;
        }

        activeCount = updateCount;
    }
}

void ParticleSystem::Play() noexcept
{
    if(state == ParticleState::NotRun)
        state = ParticleState::Run;
}

void ParticleSystem::Stop() noexcept
{
    if(state == ParticleState::Run)
        state = ParticleState::Stopped;
}

void ParticleSystem::Resume() noexcept
{
    if(state == ParticleState::Stopped)
        state = ParticleState::Run;
}

void ParticleSystem::Restart() noexcept
{
    activeTime = 0;

    if(PlayOnStart)
        state = ParticleState::Run;
    else
        state = ParticleState::NotRun;

    position = 0;
    releasePosition = 0;

    for(auto& Burst : Emission.Bursts)
        Burst.IsMake = false;
}

void ParticleSystem::SetMaxParticles(u32 maxParticles)
{
    maxParticlesCount = maxParticles;

    delete[] particles;
    particles = new Particle[maxParticlesCount]{};
    delete[] releasedParticles;
    releasedParticles = new u32[maxParticlesCount]{};

    Restart();
}

json ParticleSystem::SerializeObj()
{
    return ISerialize::SerializeObj();
}

void ParticleSystem::UnSerializeObj(const json &j)
{
    ISerialize::UnSerializeObj(j);
}
