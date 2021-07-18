#ifndef MATERIAL_H
#define MATERIAL_H

#include "ECS/ECS.h"
#include "Utils/glm.h"
#include "Assets/IAsset.h"
#include "Render/Shader.h"
#include "Assets/AssetsManager.h"
#include <boost/type_index.hpp>
#include "Utils/ShaderPackager.h"

struct Material final : public IAsset {
    std::string Shader;
    std::map<std::string, Utils::ShaderParamValue> Uniforms;
    std::map<std::string, AssetsHandle> Samplers;

    [[nodiscard]] size_t GetTypeID() const noexcept override
    {
        return boost::typeindex::type_id<Material>().hash_code();
    }

    [[nodiscard]] IAsset *Clone() const override;

    [[nodiscard]] json SerializeObj() override;
    void UnSerializeObj(const json& j) override;
};

class MaterialComponent : public ECS::Component<MaterialComponent> {
private:
    AssetsHandle materialHandle;
    Material *material = nullptr;

public:
    void SetMaterial(const AssetsHandle &materialHandle);
    [[nodiscard]] Material *GetMaterial() const noexcept { return material; }
    [[nodiscard]] AssetsHandle GetMaterialHandle() const noexcept { return materialHandle; }
    [[nodiscard]] bool IsValid() const noexcept { return material != nullptr; }

    [[nodiscard]] json SerializeObj() override;
    void UnSerializeObj(const json& j) override;

    bool Instancing = true;
};

#endif
