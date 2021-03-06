#ifndef TRANSFORM_H
#define TRANSFORM_H

#include "ECS/ECS.h"
#include "Utils/glm.h"

struct Position {
    glm::vec3 position{};
    glm::vec3 scale = glm::vec3(1.0f);
    glm::quat rotate{};
};

class Transform final : public ECS::Component<Transform> {
private:
    Position    position{};
    glm::mat4   transformMat = glm::mat4(1.0f);
    glm::vec3   up = WorldUp;
    glm::vec3   right = WorldRight;
    glm::vec3   forward = WorldFront;
    bool        localFlag = true;

    void recalculate();
    void fixParentChange();

protected:
    void UpdateTree() final;

public:
    static const glm::vec3 WorldUp;
    static const glm::vec3 WorldFront;
    static const glm::vec3 WorldRight;

    Transform()
    {
        treeLock = true;
    }

    [[nodiscard]] Position GetLocalPos() const;
    void SetLocalPos(Position p);

    [[nodiscard]] Position GetGlobalPos() const;
    void SetGlobalPos(Position p);

    [[nodiscard]] bool IsLocalPosition() const noexcept { return localFlag; }

    [[nodiscard]] inline glm::mat4 &GetMat() noexcept { return transformMat; }
    [[nodiscard]] inline glm::vec3 Right() const noexcept { return right; }
    [[nodiscard]] inline glm::vec3 Up() const noexcept { return up; }
    [[nodiscard]] inline glm::vec3 Forward() const noexcept { return forward; }

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json &j) final;
};

#endif
