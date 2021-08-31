#ifndef ASSETSMANAGER_H
#define ASSETSMANAGER_H

#include <memory>
#include <string>
#include <filesystem>
#include "Utils/json.h"
#include <unordered_map>
#include "Assets/AssetsFactory.h"

namespace fs = std::filesystem;
using AssetsHandle = std::shared_ptr<IAsset>;

class AssetsManager {
public:
    using AssetIterator = std::unordered_map<std::string, AssetsHandle>::const_iterator;

private:
    std::unordered_map<std::string, AssetsHandle> assets;
    json staticResources, dynamicResources;
    AssetsFactory assetsFactory;

public:
    AssetsManager();
    AssetsManager(const AssetsManager &assetsManager) = delete;
    AssetsManager &operator=(const AssetsManager &assetsManager) = delete;
    ~AssetsManager();

    [[nodiscard]] AssetsHandle GetAsset(const std::string &name);

    template<typename T>
    [[nodiscard]] T *GetAsset(const std::string &name)
    {
        AssetsHandle handle = GetAsset(name);

        if (handle)
            return dynamic_cast<T*>(handle.get());
        else
            return nullptr;
    }

    template<typename T>
    AssetsHandle AddAsset(const std::string &name, T *asset)
    {
        if (assets.find(name) == assets.end())
        {
            asset->name = name;
            return assets[name] = AssetsHandle(asset);
        }

        return {};
    }

    bool SaveAsset(const std::string &name, const fs::path &path);
    bool RemoveAsset(const std::string &name);
    bool DeleteAsset(const std::string &name);

    [[nodiscard]] inline AssetsFactory &GetAssetsFactory() noexcept { return assetsFactory; }

    [[nodiscard]] AssetIterator begin() const noexcept;
    [[nodiscard]] AssetIterator end() const noexcept;
};

#endif
