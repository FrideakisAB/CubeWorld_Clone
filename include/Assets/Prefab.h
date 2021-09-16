#ifndef PREFAB_H
#define PREFAB_H

#include "GameObject.h"
#include "Assets/IAsset.h"
#include <boost/type_index.hpp>

class Prefab final : public IAsset {
private:
    json prefabData;
    bool isEmpty = true;

public:
    Prefab();
    explicit Prefab(const GameObject &gameObject);
    Prefab(const Prefab &prefab);
    Prefab(Prefab &&prefab) noexcept;

    Prefab &operator=(const Prefab &prefab);
    Prefab &operator=(Prefab &&prefab) noexcept;

    [[nodiscard]] size_t GetTypeID() const noexcept final
    {
        return boost::typeindex::type_id<Prefab>().hash_code();
    }

    [[nodiscard]] IAsset *Clone() const final;

    GameObject *Use();

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json& j) final;

    void SerializeBin(std::ofstream &file) final;
    void UnSerializeBin(std::ifstream &file) final;
};

#endif
