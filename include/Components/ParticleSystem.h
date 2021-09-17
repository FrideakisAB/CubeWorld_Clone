#ifndef PARTICLESYSTEM_H
#define PARTICLESYSTEM_H

#include "ECS/ECS.h"
#include "Utils/Curve.h"
#include "Utils/Gradient.h"
#include "Assets/AssetsManager.h"
#include "Render/ParticleRender.h"

enum class EmissionType : u8 {
    OverTime = 0,
    OverDistance
};

enum class Emitter : u8 {
    Plane = 0,
    Circle,
    Sphere,
    Hemisphere,
    Cone
};

enum class ParticleState : u8 {
    Run = 0,
    Stopped,
    NotRun,
    Executed
};

struct Burst {
    f32 Time;
    u32 MinCount;
    u32 MaxCount;
    bool IsMake;
};

struct Particle {
    f32 Lifetime;
    f32 MaxLifetime;
    f32 Speed;
    f32 Size;
    f32 Sprite;
    f32 Rotation;
    f32 BrightMultiplier;
    glm::vec4 Color;
    glm::vec3 Position;
    glm::vec3 Velocity;
    bool Active;
};

class ParticleSystem final : public ECS::Component<ParticleSystem> {
private:
    bool fixStartPosition = false;
    glm::vec3 prevPosition = glm::vec3(0.0f);
    u32 activeCount = 0;
    f32 accumulateTime = 0.0f;
    f32 accumulateDistance = 0.0f;
    f32 activeTime = 0.0f;
    ParticleState state = ParticleState::NotRun;

    u32 position = 0;
    Particle *particles = nullptr;
    u32 releasePosition = 0;
    u32 *releasedParticles = nullptr;

    ParticleRender render;

    u32 maxParticlesCount = 1000;

public:
    f32 Duration = 5.0f;
    bool Loop = true;
    bool GlobalSpace = false;
    bool PlayOnStart = true;

    f32 StartDelay = 0.0f;
    f32 StartLifetime = 1.0f;
    f32 StartSpeed = 5.0f;
    f32 StartSize = 1.0f;
    f32 StartRotation = 0.0f;
    f32 StartBright = 0.0f;
    glm::vec4 StartColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

    struct Emission {
        u32 Rate = 250;
        EmissionType TypeRate = EmissionType::OverTime;
        std::vector<Burst> Bursts;
    } Emission;

    Emitter Shape = Emitter::Sphere;

    struct ColorOverLifetime {
        bool Active = false;
        Gradient Gradient;
    } ColorOverLifetime;

    struct SizeOverLifetime {
        bool Active = false;
        Curve Curve;
    } SizeOverLifetime;

    struct SpeedOverLifetime {
        bool Active = false;
        Curve Curve;
    } SpeedOverLifetime;

    struct RotationOverLifetime {
        bool Active = false;
        f32 Speed = 0.0f;
    } RotationOverLifetime;

    struct ForceOverLifetime {
        bool Active = false;
        Curve XCurve;
        Curve YCurve;
        Curve ZCurve;
        glm::vec3 BaseForce = glm::vec3(0.0f, 0.0f, 0.0f);
    } ForceOverLifetime;

    struct BrightOverLifetime {
        bool Active = false;
        Curve Curve;
        f32 BaseBright = 0.0f;
    } BrightOverLifetime;

    struct TextureOverLifetime {
        bool Active = false;
        Curve Curve;
        f32 BaseTexture = 0.0f;
    } TextureOverLifetime;

    struct ColorBySpeed {
        bool Active = false;
        Gradient Gradient;
        f32 MaxSpeed = 1.0f;
        f32 MinSpeed = 0.0f;
    } ColorBySpeed;

    struct RotationBySpeed {
        bool Active = false;
        f32 BaseSpeed = 0.0f;
        f32 MaxSpeed = 1.0f;
        f32 MinSpeed = 0.0f;
    } RotationBySpeed;

    struct SizeBySpeed {
        bool Active = false;
        Curve Curve;
        f32 BaseSize = 1.0f;
        f32 MaxSpeed = 1.0f;
        f32 MinSpeed = 0.0f;
    } SizeBySpeed;

    struct BrightBySpeed {
        bool Active = false;
        Curve Curve;
        f32 BaseBright = 0.0f;
        f32 MaxSpeed = 1.0f;
        f32 MinSpeed = 0.0f;
    } BrightBySpeed;

    struct TextureBySpeed {
        bool Active = false;
        Curve Curve;
        f32 BaseTexture = 0.0f;
        f32 MaxSpeed = 1.0f;
        f32 MinSpeed = 0.0f;
    } TextureBySpeed;

    struct ParticleTexture {
        bool Active = false;
        glm::uvec2 Tiles = glm::uvec2(1);
    } ParticleTexture;

private:
    [[nodiscard]] Particle genParticle(glm::mat4 transformMat, glm::quat transformRotate) const;

public:
    ParticleSystem();
    ~ParticleSystem() final;

    void Update();

    void Play() noexcept;
    void Stop() noexcept;
    void Resume() noexcept;
    void Restart() noexcept;

    [[nodiscard]] ParticleState GetState() const noexcept { return state; }

    [[nodiscard]] u32 GetMaxParticles() const noexcept { return maxParticlesCount; }
    void SetMaxParticles(u32 maxParticles);

    [[nodiscard]] inline u32 GetActiveParticlesCount() const noexcept { return activeCount; }

    [[nodiscard]] ParticleRender &GetRender() noexcept { return render; }

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json &j) final;
};

#endif
