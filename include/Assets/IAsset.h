#ifndef IASSET_H
#define IASSET_H

#include "Assets/ISerialize.h"
#include "Assets/IBinarySerialize.h"

class IAsset : public ISerialize, public IBinarySerialize {
    friend class AssetsManager;

private:
    bool dynamic = true;
    std::string name;

protected:
    explicit IAsset(bool binaryNeeded=false) : IBinarySerialize(binaryNeeded) {}
    IAsset(const IAsset &asset) : IBinarySerialize(asset) {}
    IAsset(IAsset &&asset)  noexcept : IBinarySerialize(asset)
    {
        std::swap(asset.name, name);
        std::swap(asset.dynamic, dynamic);
    }

public:
    virtual ~IAsset() = default;

    [[nodiscard]] virtual IAsset *Clone() const { return nullptr; }

    [[nodiscard]] bool IsDynamic() const noexcept { return dynamic; }
    [[nodiscard]] virtual size_t GetTypeID() const noexcept = 0;
    [[nodiscard]] const std::string &GetName() const noexcept { return name; }
};

#endif
