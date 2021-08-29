#include "Editor/Commands/ViewersCommands.h"

#include "Components/Camera.h"

SetCamera::SetCamera(GameObject *newCamera)
{
    if (Camera::Main != nullptr)
        goOld = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(Camera::Main->GetOwner()));
    else
        goOld = nullptr;

    goNew = newCamera;
}

void SetCamera::Execute()
{
    if (goOldId == 0 && goOld != nullptr)
        goOldId = validator.Map(goOld);
    if (goNewId == 0 && goNew != nullptr)
        goNewId = validator.Map(goNew);

    if (goNew == nullptr)
        Camera::Main = nullptr;
    else
        Camera::Main = validator.Get(goNewId)->GetComponent<Camera>();
}

void SetCamera::Undo()
{
    if (goOld == nullptr)
        Camera::Main = nullptr;
    else
        Camera::Main = validator.Get(goOldId)->GetComponent<Camera>();
}
