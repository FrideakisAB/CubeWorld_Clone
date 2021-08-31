#include "Editor/Commands/ViewersCommands.h"

#include "Components/Camera.h"
#include "Components/MeshComponent.h"

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

SetMesh::SetMesh(GameObject *gameObject, const std::string &name)
    : go(gameObject), assetName(name)
{
    AssetsHandle &handle = go->GetComponent<MeshComponent>()->GetAsset();

    if (handle)
        prevAssetName = handle->GetName();
}

void SetMesh::Execute()
{
    if (goId == 0)
        goId = validator.Map(go);

    if (!assetName.empty())
        validator.Get(goId)->GetComponent<MeshComponent>()->SetMesh(GameEngine->GetAssetsManager().GetAsset(assetName));
    else
        validator.Get(goId)->GetComponent<MeshComponent>()->SetMesh(nullptr);
}

void SetMesh::Undo()
{
    if (!prevAssetName.empty())
        validator.Get(goId)->GetComponent<MeshComponent>()->SetMesh(GameEngine->GetAssetsManager().GetAsset(prevAssetName));
    else
        validator.Get(goId)->GetComponent<MeshComponent>()->SetMesh(nullptr);
}
