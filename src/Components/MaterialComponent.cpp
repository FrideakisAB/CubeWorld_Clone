#include "Components/MaterialComponent.h"

IAsset *Material::Clone() const
{
    return new Material(std::cref(*this));
}

json Material::SerializeObj()
{
    json data;

    data["shader_name"] = Shader;
    data["uniforms_map"] = {};

    for (const auto&[name, param] : Uniforms)
    {
        json paramData;
        ns::to_json(paramData, param);
        data["uniforms_map"].emplace_back(json{name, paramData});
    }

    return data;
}

void Material::UnSerializeObj(const json &j)
{
    ISerialize::UnSerializeObj(j);
}

void MaterialComponent::SetMaterial(const AssetsHandle &materialHandle)
{
    if (auto *matPtr = dynamic_cast<Material *>(materialHandle.get()))
    {
        this->materialHandle = materialHandle;
        material = matPtr;
    }
}
