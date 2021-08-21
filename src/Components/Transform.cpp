#include "Components/Transform.h"

#include <glm/gtx/matrix_decompose.hpp>

const glm::vec3 Transform::WorldUp = glm::vec3(0.0f, 1.0f, 0.0f);
const glm::vec3 Transform::WorldFront = glm::vec3(0.0f, 0.0f, 1.0f);
const glm::vec3 Transform::WorldRight = glm::vec3(1.0f, 0.0f, 0.0f);

Position Transform::GetLocalPos() const
{
    if (localFlag)
        return position;
    else
    {
        ECS::EntityId parent = ECS::ECS_Engine->GetEntityManager()->GetEntity(owner)->GetParentID();
        if (auto trans = ECS::ECS_Engine->GetComponentManager()->GetComponent<Transform>(parent); trans != nullptr)
        {
            glm::mat4 mat = glm::inverse(trans->transformMat) * transformMat;

            Position pos;
            glm::vec3 skew;
            glm::vec4 perspective;
            glm::decompose(mat, pos.scale, pos.rotate, pos.position, skew, perspective);

            return pos;
        }
        else
            return position;
    }
}

void Transform::SetLocalPos(Position p)
{
    position = p;
    localFlag = true;
    recalculate();
}

Position Transform::GetGlobalPos() const
{
    if (!localFlag)
        return position;
    else
    {
        Position pos;
        glm::vec3 skew;
        glm::vec4 perspective;
        glm::decompose(transformMat, pos.scale, pos.rotate, pos.position, skew, perspective);

        return pos;
    }
}

void Transform::SetGlobalPos(Position p)
{
    position = p;
    localFlag = false;
    recalculate();
}

void Transform::UpdateTree()
{
    if (localFlag)
        recalculate();
}

void Transform::recalculate()
{
    auto *CM = ECS::ECS_Engine->GetComponentManager();

    glm::mat4 par = glm::mat4(1.0f);

    ECS::IEntity *entity = ECS::ECS_Engine->GetEntityManager()->GetEntity(owner);
    if (localFlag)
    {
        ECS::EntityId parent = entity->GetParentID();
        if (auto trans = CM->GetComponent<Transform>(parent); trans != nullptr)
            par = trans->transformMat;
    }

    par = glm::translate(par, position.position);
    par = par * glm::mat4_cast(position.rotate);
    par = glm::scale(par, position.scale);

    forward = position.rotate * WorldFront;
    up = position.rotate * WorldUp;
    right = position.rotate * WorldRight;

    transformMat = par;

    for(std::size_t i = 0; i < entity->GetChildCount(); ++i)
    {
        if (auto trans = CM->GetComponent<Transform>(entity->GetChildID(i)); trans != nullptr && trans->localFlag)
            trans->recalculate();
    }
}

json Transform::SerializeObj()
{
    json data;

    data["position"] = {position.position.x, position.position.y, position.position.z};
    data["rotate"] = {position.rotate.x, position.rotate.y, position.rotate.z, position.rotate.w};
    data["scale"] = {position.scale.x, position.scale.y, position.scale.z};
    data["localFlag"] = localFlag;

    return data;
}

void Transform::UnSerializeObj(const json &j)
{
    position.position = {j["position"][0], j["position"][1], j["position"][2]};
    position.rotate = {j["rotate"][0], j["rotate"][1], j["rotate"][2], j["rotate"][3]};
    position.scale = {j["scale"][0], j["scale"][1], j["scale"][2]};
    localFlag = j["localFlag"];

    recalculate();
}
