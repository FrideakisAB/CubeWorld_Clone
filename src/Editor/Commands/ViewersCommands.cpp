#include "Editor/Commands/ViewersCommands.h"

#include "Components/Camera.h"
#include "Components/MeshComponent.h"
#include "Components/MaterialComponent.h"

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

SetMaterial::SetMaterial(GameObject *gameObject, const std::string &name)
    : go(gameObject), assetName(name)
{
    if (go->GetComponent<MaterialComponent>()->IsValid())
    {
        prevAssetName = go->GetComponent<MaterialComponent>()->GetMaterial()->GetName();
        if (prevAssetName.empty())
        {
            cache = GameEditor->CacheSystem.CreateCache(8);
            std::ofstream file = std::ofstream(fs::current_path().string() + cache.GetPath());
            std::string jsonStr = go->GetComponent<MaterialComponent>()->GetMaterial()->SerializeObj().dump(4);
            file.write(jsonStr.c_str(), jsonStr.size());
            file.close();
        }
    }
}

SetMaterial::~SetMaterial()
{
    cache = GameEditor->CacheSystem.GetCache(cache.GetID());
    if (!cache.GetPath().empty())
        GameEditor->CacheSystem.RemoveCache(cache);
}

void SetMaterial::Execute()
{
    if (goId == 0)
        goId = validator.Map(go);

    if (!assetName.empty())
        validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(GameEngine->GetAssetsManager().GetAsset(assetName));
    else
        validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial({});
}

void SetMaterial::Undo()
{
    if (!prevAssetName.empty())
        validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(GameEngine->GetAssetsManager().GetAsset(prevAssetName));
    else
    {
        cache = GameEditor->CacheSystem.GetCache(cache.GetID());
        if (!cache.GetPath().empty())
        {
            std::ifstream file = std::ifstream(fs::current_path().string() + cache.GetPath());
            auto materialHandle = std::make_shared<Material>();
            auto *material = static_cast<Material*>(materialHandle.get());
            material->UnSerializeObj(json_utils::TryParse(Utils::FileToString(std::move(file))));
            validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(materialHandle);
        }
        else
            validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial({});
    }
}

SetRawMaterial::SetRawMaterial(GameObject *gameObject, Material *material)
    : go(gameObject), material(material)
{
    if (go->GetComponent<MaterialComponent>()->IsValid())
    {
        prevAssetName = go->GetComponent<MaterialComponent>()->GetMaterial()->GetName();
        if (prevAssetName.empty())
        {
            prevCache = GameEditor->CacheSystem.CreateCache(8);
            std::ofstream file = std::ofstream(fs::current_path().string() + prevCache.GetPath());
            std::string jsonStr = go->GetComponent<MaterialComponent>()->GetMaterial()->SerializeObj().dump(4);
            file.write(jsonStr.c_str(), jsonStr.size());
            file.close();
        }
    }

    newCache = GameEditor->CacheSystem.CreateCache(8);
    std::ofstream file = std::ofstream(fs::current_path().string() + newCache.GetPath());
    std::string jsonStr = material->SerializeObj().dump(4);
    file.write(jsonStr.c_str(), jsonStr.size());
    file.close();
}

SetRawMaterial::~SetRawMaterial()
{
    prevCache = GameEditor->CacheSystem.GetCache(prevCache.GetID());
    if (!prevCache.GetPath().empty())
        GameEditor->CacheSystem.RemoveCache(prevCache);

    newCache = GameEditor->CacheSystem.GetCache(newCache.GetID());
    if (!newCache.GetPath().empty())
        GameEditor->CacheSystem.RemoveCache(newCache);
}

void SetRawMaterial::Execute()
{
    if (goId == 0)
        goId = validator.Map(go);

    if (first)
    {
        validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(AssetsHandle(material));
        first = false;
    }
    else
    {
        if (!newCache.GetPath().empty())
        {
            std::ifstream file = std::ifstream(fs::current_path().string() + newCache.GetPath());
            auto materialHandle = std::make_shared<Material>();
            auto *materialPtr = static_cast<Material*>(materialHandle.get());
            materialPtr->UnSerializeObj(json_utils::TryParse(Utils::FileToString(std::move(file))));
            validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(materialHandle);
        }
    }
}

void SetRawMaterial::Undo()
{
    if (!prevAssetName.empty())
        validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(GameEngine->GetAssetsManager().GetAsset(prevAssetName));
    else
    {
        prevCache = GameEditor->CacheSystem.GetCache(prevCache.GetID());
        if (!prevCache.GetPath().empty())
        {
            std::ifstream file = std::ifstream(fs::current_path().string() + prevCache.GetPath());
            auto materialHandle = std::make_shared<Material>();
            auto *materialPtr = static_cast<Material*>(materialHandle.get());
            materialPtr->UnSerializeObj(json_utils::TryParse(Utils::FileToString(std::move(file))));
            validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial(materialHandle);
        }
        else
            validator.Get(goId)->GetComponent<MaterialComponent>()->SetMaterial({});
    }
}
