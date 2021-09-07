#include "Assets/AssetsManager.h"

AssetsManager::AssetsManager()
{
    staticResources = json_utils::TryParse(Utils::FileToString(std::ifstream("data/staticReference.db")));
    dynamicResources = json_utils::TryParse(Utils::FileToString(std::ifstream("data/dynamicReference.db")));
}

AssetsManager::~AssetsManager()
{
    try
    {
        std::ofstream dynamicDBfile("data/dynamicReference.db", std::ios::trunc);
        dynamicDBfile << dynamicResources.dump(4);
        dynamicDBfile.close();
    }
    catch (...)
    {
        logger->Error("Dynamic resources not save! Error in AssetsManager");
    }
}

AssetsHandle AssetsManager::GetAsset(const std::string &name)
{
    if (assets.find(name) != assets.end())
        return assets[name];
    else if (staticResources.contains(name))
    {
        json assetData = json_utils::TryParse(Utils::FileToString(std::ifstream(staticResources[name][0].get<std::string>())));

        auto *asset = assetsFactory.Create(assetData["nameType"].get<std::string>());
        asset->UnSerializeObj(assetData["data"]);
        if (asset->IsBinaryNeeded() && !staticResources[name][1].get<std::string>().empty())
        {
            std::ifstream file = std::ifstream(staticResources[name][1].get<std::string>());
            if (file.is_open())
            {
                asset->UnSerializeBin(file);
                file.close();
            }
        }
        asset->dynamic = false;
        asset->name = name;

        assets[name] = AssetsHandle(asset);

        return assets[name];
    }
    else if (dynamicResources.contains(name))
    {
        json assetData = json_utils::TryParse(Utils::FileToString(std::ifstream(dynamicResources[name][0].get<std::string>())));

        auto* asset = assetsFactory.Create(assetData["nameType"].get<std::string>());
        asset->UnSerializeObj(assetData["data"]);
        if (asset->IsBinaryNeeded() && !dynamicResources[name][1].get<std::string>().empty())
        {
            std::ifstream file = std::ifstream(dynamicResources[name][1].get<std::string>());
            if (file.is_open())
            {
                asset->UnSerializeBin(file);
                file.close();
            }
        }
        asset->name = name;

        assets[name] = AssetsHandle(asset);

        return assets[name];
    }

    return {};
}

bool AssetsManager::SaveAsset(const std::string &name, const fs::path &path)
{
    if (auto It = assets.find(name); It != assets.end() && It->second->IsDynamic())
    {
        dynamicResources[name] = {path.string(), ""};
        std::ofstream file = std::ofstream(path.string(), std::ios_base::trunc);
        if (file.is_open())
        {
            file << It->second->SerializeObj().dump(4);
            file.close();

            if (It->second->IsBinaryNeeded())
            {
                file.open(path.string() + ".bin", std::ios_base::trunc);
                if (file.is_open())
                {
                    It->second->SerializeBin(file);
                    return true;
                }
            }
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
    {
        bool result = fs::remove(dynamicResources[name][0].get<std::string>());

        if (!dynamicResources[name][1].get<std::string>().empty())
            result = result && fs::remove(dynamicResources[name][1].get<std::string>());

        return result;
    }

    return false;
}

AssetsManager::AssetIterator AssetsManager::begin() const noexcept
{
    return assets.cbegin();
}

AssetsManager::AssetIterator AssetsManager::end() const noexcept
{
    return assets.cend();
}

void AssetsManager::AddAsset(const std::string &name, const AssetsHandle &asset)
{
    if (assets.find(name) == assets.end())
    {
        asset->name = name;
        assets[name] = asset;
    }
}
