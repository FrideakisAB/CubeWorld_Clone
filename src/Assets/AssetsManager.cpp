#include "Assets/AssetsManager.h"

AssetsManager::AssetsManager()
{

}

AssetsManager::~AssetsManager()
{

}

AssetsHandle AssetsManager::GetAsset(const std::string &name)
{
    if(assets.find(name) != assets.end())
        return assets[name];
    else if(staticResources.contains(name))
    {
        json assetData = json_utils::TryParse(Utils::FileToString(std::ifstream(staticResources[name].get<std::string>())));

        auto *asset = assetsFactory.Create(assetData["nameType"].get<std::string>());
        asset->UnSerializeObj(assetData["data"]);
        asset->dynamic = false;

        assets[name] = AssetsHandle(asset);

        return assets[name];
    }
    else if(dynamicResources.contains(name))
    {
        json assetData = json_utils::TryParse(Utils::FileToString(std::ifstream(dynamicResources[name].get<std::string>())));

        auto* asset = assetsFactory.Create(assetData["nameType"].get<std::string>());
        asset->UnSerializeObj(assetData["data"]);

        assets[name] = AssetsHandle(asset);

        return assets[name];
    }

    return AssetsHandle();
}

bool AssetsManager::SaveAsset(const std::string &name, const fs::path &path)
{
    if (auto It = assets.find(name); It != assets.end() && It->second->IsDynamic())
    {
        dynamicResources[name] = path;
        std::ofstream file = std::ofstream(path.string(), std::ios_base::trunc);
        if (file.is_open())
        {
            file << It->second->SerializeObj().dump(4);
            file.close();

            return true;
        }
    }

    return false;
}

bool AssetsManager::RemoveAsset(const std::string &name)
{
    if (auto It = assets.find(name); It != assets.end() && It->second->IsDynamic())
    {
        assets.erase(name);

        return true;
    }

    return false;
}

bool AssetsManager::DeleteAsset(const std::string &name)
{
    if (auto It = assets.find(name); It != assets.end() && It->second->IsDynamic() && dynamicResources.contains(name))
        return fs::remove(dynamicResources[name].get<std::string>());

    return false;
}

auto AssetsManager::begin() const noexcept
{
    return assets.cbegin();
}

auto AssetsManager::end() const noexcept
{
    return assets.cend();
}
