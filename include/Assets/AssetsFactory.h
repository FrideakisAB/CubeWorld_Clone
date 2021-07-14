#ifndef ASSETSFACTORY_H
#define ASSETSFACTORY_H

#include "Log.h"
#include <string>
#include <unordered_map>
#include "Assets/IAsset.h"
#include <boost/type_index.hpp>

class AssetsFactory {
    class IAssetSet {
    public:
        virtual IAsset *Add() = 0;
        virtual ~IAssetSet() = default;
    };

    template<class T>
    class AssetSet : public IAssetSet {
    public:
        IAsset *Add() override
        {
            return new T();
        }
    };

    using AstSetRegistry = std::unordered_map<std::string, IAssetSet *>;

private:
    AstSetRegistry astSetRegistry;

public:
    ~AssetsFactory()
    {
        for (auto ar : astSetRegistry)
        {
            delete ar.second;
            ar.second = nullptr;
        }

        astSetRegistry.clear();
    }

    template<class T>
    std::string Register()
    {
        std::string ASID = boost::typeindex::type_id<T>().pretty_name();

        if (astSetRegistry.find(ASID) == astSetRegistry.end())
            astSetRegistry[ASID] = new AssetSet<T>();

        return ASID;
    }

    IAsset *Create(const std::string &name)
    {
        return astSetRegistry[name]->Add();
    }
};

#endif
