#ifndef MESHCOMPONENT_H
#define MESHCOMPONENT_H

#include "ECS/ECS.h"
#include "Render/Mesh.h"
#include <boost/type_index.hpp>
#include "Assets/AssetsManager.h"

class MeshComponent : public ECS::Component<MeshComponent> {
private:
    AssetsHandle meshHandle;
    Mesh *mesh = nullptr;

public:
    MeshComponent() = default;
    MeshComponent(const MeshComponent& meshComponent)
    {
        meshHandle = meshComponent.meshHandle;
    }

    inline Mesh& GetMesh() { return *mesh; }
    inline AssetsHandle& GetAsset() { return meshHandle; }

    void SetMesh(Mesh* newMesh)
    {
        if (newMesh != nullptr)
        {
            meshHandle = AssetsHandle(newMesh);
            mesh = newMesh;
        }
        else
        {
            meshHandle = {};
            mesh = nullptr;
        }
    }

    void SetMesh(const AssetsHandle& newMesh)
    {
        if(auto *meshPtr = dynamic_cast<Mesh *>(newMesh.get()))
        {
            meshHandle = newMesh;
            mesh = meshPtr;
        }
        else
        {
            meshHandle = {};
            mesh = nullptr;
        }
    }

    [[nodiscard]] bool IsValid() const noexcept { return mesh != nullptr; }

    json SerializeObj() override
    {
        json data;

        data["cmpName"] = boost::typeindex::type_id<MeshComponent>().pretty_name();

        if(meshHandle && !meshHandle->GetName().empty())
            data["mesh"] = meshHandle->GetName();
        else
            data["mesh"] = "";

        return data;
    }

    void UnSerializeObj(const json& j) override
    {
        //TODO: on singleton implementation
    }
};

#endif
