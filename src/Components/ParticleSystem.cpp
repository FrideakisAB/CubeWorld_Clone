#include "Components/ParticleSystem.h"

#include "Utils/Math.h"
#include "Utils/Mathf.h"
#include "Utils/Curve.h"
#include "ECS/util/Timer.h"
#include "Components/Transform.h"

ParticleSystem::ParticleSystem()
{
    render.Initialize(this);
    particles = new Particle[maxParticlesCount];
    releasedParticles = new u32[maxParticlesCount];

    if (PlayOnStart)
        state = ParticleState::Run;
}

ParticleSystem::~ParticleSystem()
{
    Emission.Bursts.clear();

    delete[] releasedParticles;
    delete[] particles;
}

Particle ParticleSystem::genParticle(glm::mat4 transformMat, glm::quat transformRotate) const
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
    particle.Sprite = 0.0f;

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

    case Emitter::Hemisphere:
    {
        f32 a1 = Mathf::RangeRandom(0.0f, 180.0f);
        f32 a2 = Mathf::RangeRandom(0.0f, 180.0f);
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

    if (GlobalSpace)
    {
        particle.Position = transformMat * glm::vec4(particle.Position, 1.0f);
        particle.Velocity = transformRotate * glm::vec4(particle.Velocity, 1.0f);
    }

    return particle;
}

void ParticleSystem::Update()
{
    f32 deltaTime = ECS::ECS_Engine->GetTimer()->GetDeltaTime();
    if (state == ParticleState::Run)
        activeTime += deltaTime;

    if (activeTime > StartDelay && state == ParticleState::Run && (activeTime < Duration || Loop))
    {
        Position newPosition{};
        glm::mat4 transformMat{};
        glm::quat transformRotate{};

        if (GlobalSpace || Emission.TypeRate == EmissionType::OverDistance)
        {
            auto *entity = ECS::ECS_Engine->GetEntityManager()->GetEntity(GetOwner());
            auto *transform = entity->GetComponent<Transform>();

            newPosition = transform->GetGlobalPos();
            transformMat = transform->GetMat();
            transformRotate = newPosition.rotate;

            if (!fixStartPosition)
            {
                prevPosition = newPosition.position;
                fixStartPosition = true;
            }
        }

        for (auto &Burst : Emission.Bursts)
        {
            if (Burst.Time <= activeTime && !Burst.IsMake)
            {
                u32 particleCount = Math::RangeRandom(Burst.MinCount, Burst.MaxCount);
                if (particleCount + position > maxParticlesCount)
                    particleCount = maxParticlesCount - position;

                for (u32 i = 0; i < particleCount; ++i)
                {
                    Particle part = genParticle(transformMat, transformRotate);
                    particles[position++] = part;
                }

                Burst.IsMake = true;
            }
        }

        if (Emission.TypeRate == EmissionType::OverTime)
        {
            auto count = static_cast<u32>((deltaTime + accumulateTime) * (f32)Emission.Rate);
            if (count != 0)
            {
                if (Loop && releasePosition > 0)
                {
                    for (u32 i = releasePosition; i > 0 && count > 0; --i)
                    {
                        Particle part = genParticle(transformMat, transformRotate);
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
                    Particle part = genParticle(transformMat, transformRotate);
                    particles[position++] = part;
                }

                accumulateTime = 0.0f;
            }
            else
                accumulateTime += deltaTime;
        }
        else if (Emission.TypeRate == EmissionType::OverDistance)
        {
            f32 distance = abs(glm::distance(prevPosition, newPosition.position));
            prevPosition = newPosition.position;
            auto count = static_cast<u32>((distance + accumulateDistance) * (f32)Emission.Rate);
            if (count != 0)
            {
                if (Loop && releasePosition > 0)
                {
                    for (u32 i = releasePosition; i > 0 && count > 0; --i)
                    {
                        Particle part = genParticle(transformMat, transformRotate);
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
                    Particle part = genParticle(transformMat, transformRotate);
                    particles[position++] = part;
                }

                accumulateDistance = 0.0f;
            }
            else
                accumulateDistance += distance;
        }

        u32 updateCount = 0;
        for (u32 i = 0; i < position; ++i)
        {
            if (!particles[i].Active)
                continue;

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

            if (SizeOverLifetime.Active)
                particles[i].Size = (StartSize / 10) * SizeOverLifetime.Curve.Value(particleLifetimeAspect);
            else if (SizeBySpeed.Active && SizeBySpeed.MinSpeed != SizeBySpeed.MaxSpeed)
            {
                f32 speedAspect = (particles[i].Speed - SizeBySpeed.MinSpeed) / (SizeBySpeed.MaxSpeed - SizeBySpeed.MinSpeed);
                particles[i].Size = SizeBySpeed.BaseSize * SizeBySpeed.Curve.Value(speedAspect);
            }

            if (SpeedOverLifetime.Active)
                particles[i].Speed = StartSpeed * SpeedOverLifetime.Curve.Value(particleLifetimeAspect);

            if (RotationOverLifetime.Active)
                particles[i].Rotation += RotationOverLifetime.Speed * deltaTime;

            particles[i].Position += particles[i].Velocity * particles[i].Speed * deltaTime;

            if (ForceOverLifetime.Active)
                particles[i].Position += ForceOverLifetime.BaseForce * glm::vec3(
                        ForceOverLifetime.XCurve.Value(particleLifetimeAspect),
                        ForceOverLifetime.YCurve.Value(particleLifetimeAspect),
                        ForceOverLifetime.ZCurve.Value(particleLifetimeAspect));

            if (BrightOverLifetime.Active)
                particles[i].BrightMultiplier = BrightOverLifetime.BaseBright * BrightOverLifetime.Curve.Value(particleLifetimeAspect);
            else if (BrightBySpeed.Active && BrightBySpeed.MinSpeed != BrightBySpeed.MaxSpeed)
            {
                f32 speedAspect = (particles[i].Speed - BrightBySpeed.MinSpeed) / (BrightBySpeed.MaxSpeed - BrightBySpeed.MinSpeed);
                particles[i].BrightMultiplier = BrightBySpeed.BaseBright * BrightBySpeed.Curve.Value(speedAspect);
            }

            if (TextureOverLifetime.Active)
                particles[i].Sprite = TextureOverLifetime.BaseTexture * TextureOverLifetime.Curve.Value(particleLifetimeAspect);
            else if (TextureBySpeed.Active)
            {
                f32 speedAspect = (particles[i].Speed - TextureBySpeed.MinSpeed) / (TextureBySpeed.MaxSpeed - TextureBySpeed.MinSpeed);
                particles[i].Sprite = TextureBySpeed.BaseTexture * TextureBySpeed.Curve.Value(speedAspect);
            }

            if (RotationBySpeed.Active)
            {
                if (particles[i].Speed >= RotationBySpeed.MinSpeed && particles[i].Speed <= RotationBySpeed.MaxSpeed)
                    particles[i].Rotation += RotationBySpeed.BaseSpeed * deltaTime;
            }

            particles[i].Lifetime += deltaTime;
            if (particles[i].Lifetime > particles[i].MaxLifetime)
            {
                if(Loop)
                    releasedParticles[releasePosition++] = i;

                particles[i].Active = false;
            }

            ++updateCount;
        }

        activeCount = updateCount;
    }
    else if (activeTime > Duration && !Loop)
    {
        position = 0;
        state = ParticleState::Executed;
    }

    render.UpdateData(maxParticlesCount, particles, position);
}

void ParticleSystem::Play() noexcept
{
    if (state == ParticleState::NotRun)
    {
        state = ParticleState::Run;
        fixStartPosition = false;
    }
}

void ParticleSystem::Stop() noexcept
{
    if (state == ParticleState::Run)
        state = ParticleState::Stopped;
}

void ParticleSystem::Resume() noexcept
{
    if (state == ParticleState::Stopped)
    {
        state = ParticleState::Run;
        fixStartPosition = false;
    }
}

void ParticleSystem::Restart() noexcept
{
    activeTime = 0;
    fixStartPosition = false;

    if (PlayOnStart)
        state = ParticleState::Run;
    else
        state = ParticleState::NotRun;

    position = 0;
    releasePosition = 0;

    for (auto &Burst : Emission.Bursts)
        Burst.IsMake = false;

    render.UpdateData(maxParticlesCount, particles, position);
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

json ParticleSystem::SerializeObj() const
{
    json data;

    data["cmpName"] = boost::typeindex::type_id<ParticleSystem>().pretty_name();

    data["Duration"] = Duration;
    data["MaxParticles"] = maxParticlesCount;
    data["Loop"] = Loop;
    data["GlobalSpace"] = GlobalSpace;
    data["PlayOnStart"] = PlayOnStart;

    data["StartDelay"] = StartDelay;
    data["StartLifetime"] = StartLifetime;
    data["StartSpeed"] = StartSpeed;
    data["StartSize"] = StartSize;
    data["StartRotation"] = StartRotation;
    data["StartColor"] = {StartColor.x, StartColor.y, StartColor.z, StartColor.w};

    data["Rate"] = Emission.Rate;
    data["TypeRate"] = static_cast<u8>(Emission.TypeRate);
    data["burstsCount"] = Emission.Bursts.size();
    for (u32 i = 0; i < Emission.Bursts.size(); ++i)
        data["bursts"][i] = {Emission.Bursts[i].Time, Emission.Bursts[i].MinCount, Emission.Bursts[i].MaxCount, Emission.Bursts[i].IsMake};

    data["Shape"] = static_cast<u8>(Shape);

    data["ColorOverLifetime_Active"] = ColorOverLifetime.Active;
    data["ColorOverLifetime_Gradient"] = ColorOverLifetime.Gradient.SerializeObj();

    data["SizeOverLifetime_Active"] = SizeOverLifetime.Active;
    data["SizeOverLifetime_Curve"] = SizeOverLifetime.Curve.SerializeObj();

    data["SpeedOverLifetime_Active"] = SpeedOverLifetime.Active;
    data["SpeedOverLifetime_Curve"] = SpeedOverLifetime.Curve.SerializeObj();

    data["RotationOverLifetime_Active"] = RotationOverLifetime.Active;
    data["RotationOverLifetime_Speed"] = RotationOverLifetime.Speed;

    data["ForceOverLifetime_Active"] = ForceOverLifetime.Active;
    data["ForceOverLifetime_XCurve"] = ForceOverLifetime.XCurve.SerializeObj();
    data["ForceOverLifetime_YCurve"] = ForceOverLifetime.YCurve.SerializeObj();
    data["ForceOverLifetime_ZCurve"] = ForceOverLifetime.ZCurve.SerializeObj();
    data["ForceOverLifetime_BaseForce"] = {ForceOverLifetime.BaseForce.x, ForceOverLifetime.BaseForce.y, ForceOverLifetime.BaseForce.z};

    data["BrightOverLifetime_Active"] = BrightOverLifetime.Active;
    data["BrightOverLifetime_Curve"] = BrightOverLifetime.Curve.SerializeObj();
    data["BrightOverLifetime_BaseBright"] = BrightOverLifetime.BaseBright;

    data["TextureOverLifetime_Active"] = TextureOverLifetime.Active;
    data["TextureOverLifetime_Curve"] = TextureOverLifetime.Curve.SerializeObj();
    data["TextureOverLifetime_BaseTexture"] = TextureOverLifetime.BaseTexture;

    data["ColorBySpeed_Active"] = ColorBySpeed.Active;
    data["ColorBySpeed_MaxSpeed"] = ColorBySpeed.MaxSpeed;
    data["ColorBySpeed_MinSpeed"] = ColorBySpeed.MinSpeed;
    data["ColorBySpeed_Gradient"] = ColorBySpeed.Gradient.SerializeObj();

    data["RotationBySpeed_Active"] = RotationBySpeed.Active;
    data["RotationBySpeed_MaxSpeed"] = RotationBySpeed.MaxSpeed;
    data["RotationBySpeed_MinSpeed"] = RotationBySpeed.MinSpeed;
    data["RotationBySpeed_BaseSpeed"] = RotationBySpeed.BaseSpeed;

    data["SizeBySpeed_Active"] = SizeBySpeed.Active;
    data["SizeBySpeed_MaxSpeed"] = SizeBySpeed.MaxSpeed;
    data["SizeBySpeed_MinSpeed"] = SizeBySpeed.MinSpeed;
    data["SizeBySpeed_BaseSize"] = SizeBySpeed.BaseSize;

    data["BrightBySpeed_Active"] = BrightBySpeed.Active;
    data["BrightBySpeed_MaxSpeed"] = BrightBySpeed.MaxSpeed;
    data["BrightBySpeed_MinSpeed"] = BrightBySpeed.MinSpeed;
    data["BrightBySpeed_BaseBright"] = BrightBySpeed.BaseBright;

    data["TextureBySpeed_Active"] = TextureBySpeed.Active;
    data["TextureBySpeed_MaxSpeed"] = TextureBySpeed.MaxSpeed;
    data["TextureBySpeed_MinSpeed"] = TextureBySpeed.MinSpeed;
    data["TextureBySpeed_BaseTexture"] = TextureBySpeed.BaseTexture;

    data["State"] = static_cast<u8>(state);

    data["ParticleTexture_Active"] = ParticleTexture.Active;
    data["ParticleTexture_Tiles"] = {ParticleTexture.Tiles.x, ParticleTexture.Tiles.y};

    return data;
}

void ParticleSystem::UnSerializeObj(const json &j)
{
    Duration = j["Duration"];
    Loop = j["Loop"];
    GlobalSpace = j["GlobalSpace"];
    PlayOnStart = j["PlayOnStart"];

    StartDelay = j["StartDelay"];
    StartLifetime = j["StartLifetime"];
    StartSpeed = j["StartSpeed"];
    StartSize = j["StartSize"];
    StartRotation = j["StartRotation"];
    StartColor = glm::vec4(j["StartColor"][0], j["StartColor"][1], j["StartColor"][2], j["StartColor"][3]);

    Emission.Rate = j["Rate"];
    Emission.TypeRate = static_cast<EmissionType>(j["TypeRate"]);
    Emission.Bursts.clear();
    size_t burstSize = j["burstsCount"];
    Emission.Bursts.reserve(burstSize);
    for (u32 i = 0; i < burstSize; ++i)
        Emission.Bursts.push_back({j["bursts"][i][0], j["bursts"][i][1], j["bursts"][i][2], j["bursts"][i][3]});

    Shape = static_cast<Emitter>(j["Shape"]);

    ColorOverLifetime.Active = j["ColorOverLifetime_Active"];
    ColorOverLifetime.Gradient.UnSerializeObj(j["ColorOverLifetime_Gradient"]);

    SizeOverLifetime.Active = j["SizeOverLifetime_Active"];
    SizeOverLifetime.Curve.UnSerializeObj(j["SizeOverLifetime_Curve"]);

    SpeedOverLifetime.Active = j["SpeedOverLifetime_Active"];
    SpeedOverLifetime.Curve.UnSerializeObj(j["SpeedOverLifetime_Curve"]);

    RotationOverLifetime.Active = j["RotationOverLifetime_Active"];
    RotationOverLifetime.Speed = j["RotationOverLifetime_Speed"];

    ForceOverLifetime.Active = j["ForceOverLifetime_Active"];
    ForceOverLifetime.XCurve.UnSerializeObj(j["ForceOverLifetime_XCurve"]);
    ForceOverLifetime.YCurve.UnSerializeObj(j["ForceOverLifetime_YCurve"]);
    ForceOverLifetime.ZCurve.UnSerializeObj(j["ForceOverLifetime_ZCurve"]);
    ForceOverLifetime.BaseForce = glm::vec3(j["ForceOverLifetime_BaseForce"][0], j["ForceOverLifetime_BaseForce"][1], j["ForceOverLifetime_BaseForce"][2]);

    BrightOverLifetime.Active = j["BrightOverLifetime_Active"];
    BrightOverLifetime.Curve.UnSerializeObj(j["BrightOverLifetime_Curve"]);
    BrightOverLifetime.BaseBright = j["BrightOverLifetime_BaseBright"];

    TextureOverLifetime.Active = j["TextureOverLifetime_Active"];
    TextureOverLifetime.Curve.UnSerializeObj(j["TextureOverLifetime_Curve"]);
    TextureOverLifetime.BaseTexture = j["TextureOverLifetime_BaseTexture"];

    ColorBySpeed.Active = j["ColorBySpeed_Active"];
    ColorBySpeed.MaxSpeed = j["ColorBySpeed_MaxSpeed"];
    ColorBySpeed.MinSpeed = j["ColorBySpeed_MinSpeed"];
    ColorBySpeed.Gradient.UnSerializeObj(j["ColorBySpeed_Gradient"]);

    RotationBySpeed.Active = j["RotationBySpeed_Active"];
    RotationBySpeed.MaxSpeed = j["RotationBySpeed_MaxSpeed"];
    RotationBySpeed.MinSpeed = j["RotationBySpeed_MinSpeed"];
    RotationBySpeed.BaseSpeed = j["RotationBySpeed_BaseSpeed"];

    SizeBySpeed.Active = j["SizeBySpeed_Active"];
    SizeBySpeed.MaxSpeed = j["SizeBySpeed_MaxSpeed"];
    SizeBySpeed.MinSpeed = j["SizeBySpeed_MinSpeed"];
    SizeBySpeed.BaseSize = j["SizeBySpeed_BaseSize"];

    BrightBySpeed.Active = j["BrightBySpeed_Active"];
    BrightBySpeed.MaxSpeed = j["BrightBySpeed_MaxSpeed"];
    BrightBySpeed.MinSpeed = j["BrightBySpeed_MinSpeed"];
    BrightBySpeed.BaseBright = j["BrightBySpeed_BaseBright"];

    TextureBySpeed.Active = j["TextureBySpeed_Active"];
    TextureBySpeed.MaxSpeed = j["TextureBySpeed_MaxSpeed"];
    TextureBySpeed.MinSpeed = j["TextureBySpeed_MinSpeed"];
    TextureBySpeed.BaseTexture = j["TextureBySpeed_BaseTexture"];

    state = static_cast<ParticleState>(j["State"]);

    ParticleTexture.Active = j["ParticleTexture_Active"];
    ParticleTexture.Tiles = glm::uvec2(j["ParticleTexture_Tiles"][0], j["ParticleTexture_Tiles"][1]);

    SetMaxParticles(j["MaxParticles"]);
}
