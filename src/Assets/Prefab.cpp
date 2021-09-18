#include "Assets/Prefab.h"

#include "Engine.h"
#include "GameScene.h"
#include "Components/Transform.h"

Prefab::Prefab()
    : IAsset(true)
{}

Prefab::Prefab(const GameObject &gameObject)
    : IAsset(true)
{
    prefabData = gameObject.SerializeObj();
}

Prefab::Prefab(const Prefab &prefab)
    : IAsset(true)
{
    prefabData = prefab.prefabData;
}

Prefab::Prefab(Prefab &&prefab) noexcept
    : IAsset(true)
{
    std::swap(prefabData, prefab.prefabData);
}

Prefab &Prefab::operator=(const Prefab &prefab)
{
    prefabData = prefab.prefabData;
    return *this;
}

Prefab &Prefab::operator=(Prefab &&prefab) noexcept
{
    std::swap(prefabData, prefab.prefabData);
    return *this;
}

IAsset *Prefab::Clone() const
{
    return new Prefab(std::cref(*this));
}

GameObject *Prefab::Use()
{
    GameObject *go;
    go = GameEngine->GetGameScene().Create(GetName());
    if (prefabData.empty())
        go->AddComponent<Transform>();
    else
    {
        go->UnSerializeObj(prefabData);
        go->Name = GetName();
    }
    go->isPrefab = true;

    return go;
}

json Prefab::SerializeObj() const
{
    json data;

    data["nameType"] = boost::typeindex::type_id<Prefab>().pretty_name();

    data["isEmpty"] = prefabData.empty();

    return data;
}

void Prefab::UnSerializeObj(const json &j)
{
    isEmpty = j["isEmpty"];
}

void Prefab::SerializeBin(std::ofstream &file)
{
    if (!prefabData.empty())
    {
        std::string data = prefabData.dump();
        file.write(data.c_str(), data.size());
    }
}

void Prefab::UnSerializeBin(std::ifstream &file)
{
    if (!isEmpty)
        prefabData = json_utils::TryParse(Utils::FileToString(file));
}