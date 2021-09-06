#ifndef ASSETSFACTORY_H
#define ASSETSFACTORY_H

#include "Log.h"
#include <string>
#include <unordered_map>
#include "Assets/IAsset.h"
#include <boost/type_index.hpp>

class AssetsFactory {
    friend class AssetsViewer;

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

    using AstSetRegistry = std::unordered_map<std::string, std::pair<IAssetSet*, size_t>>;

private:
    AstSetRegistry astSetRegistry;

public:
    ~AssetsFactory()
    {
        for (auto ar : astSetRegistry)
        {
            delete ar.second.first;
            ar.second.first = nullptr;
        }

        astSetRegistry.clear();
    }

    template<class T>
    std::string Register()
    {
        std::string ASID = boost::typeindex::type_id<T>().pretty_name();

        if (astSetRegistry.find(ASID) == astSetRegistry.end())
        {
            astSetRegistry[ASID].first = new AssetSet<T>();
            astSetRegistry[ASID].second = boost::typeindex::type_id<T>().hash_code();
        }

        return ASID;
    }

    IAsset *Create(const std::string &name)
    {
        return astSetRegistry[name].first->Add();
    }
};

#endif
