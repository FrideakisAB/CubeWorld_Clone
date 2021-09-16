#ifndef ASSETVIEWERSREGISTER_H
#define ASSETVIEWERSREGISTER_H

#include <map>
#include <boost/type_index.hpp>
#include "Editor/UI/Viewers/IAssetViewer.h"

class AssetViewersRegister {
private:
    std::map<size_t, IAssetViewer*> viewers;

public:
    ~AssetViewersRegister()
    {
        for (auto &&[id, viewer] : viewers)
            delete viewer;
    }

    template<class Vwr, class Asset>
    void RegisterViewer()
    {
        size_t hashCode = boost::typeindex::type_id<Asset>().hash_code();
        if (viewers.find(hashCode) != viewers.end())
            delete viewers[hashCode];

        viewers[hashCode] = new Vwr();
    }

    [[nodiscard]] IAssetViewer *GetViewer(size_t id) const
    {
        if (auto It = viewers.find(id); It != viewers.end())
            return It->second;
        else
            return nullptr;
    }
};

#endif
