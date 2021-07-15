#ifndef IASSET_H
#define IASSET_H

#include "ECS/Serialization/ISerialize.h"

class IAsset : public ISerialize {
    friend class AssetsManager;

private:
    bool dynamic = true;

public:
    virtual ~IAsset() = default;

    [[nodiscard]] virtual IAsset* Clone() const { return nullptr; }

    [[nodiscard]] bool IsDynamic() const noexcept { return dynamic; }
    [[nodiscard]] virtual size_t GetTypeID() const noexcept = 0;
};

#endif
