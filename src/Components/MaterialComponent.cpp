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
    data["samplers_map"] = {};

    for (const auto&[name, param] : Uniforms)
    {
        json paramData;
        ns::to_json(paramData, param);
        data["uniforms_map"].emplace_back(json{name, paramData});
    }

    for (const auto&[name, handle] : Samplers)
        if (!handle->GetName().empty())
            data["samplers_map"].emplace_back(json{name, handle->GetName()});

    return data;
}

void Material::UnSerializeObj(const json &j)
{
    Shader = j["shader_name"];

    for (auto &elm : j["uniforms_map"].items())
    {
        Utils::ShaderParamValue param;
        ns::from_json(elm.value()[1], param);
        Uniforms[elm.value()[0].get<std::string>()] = param;
    }

    for (auto &elm : j["samplers_map"].items())
    {
        //TODO: on singleton implementation
        //if (auto asset = AssetsManager.GetAsset(elm.value()[1].get<std::string>()))
        //    Samplers[elm.value()[0].get<std::string>()] = std::move(asset);
    }
}

void MaterialComponent::SetMaterial(const AssetsHandle &materialHandle)
{
    if (auto *matPtr = dynamic_cast<Material *>(materialHandle.get()))
    {
        this->materialHandle = materialHandle;
        material = matPtr;
    }
}

json MaterialComponent::SerializeObj()
{
    json data;

    if(material != nullptr)
    {
        if (!materialHandle->GetName().empty())
            data["materialName"] = materialHandle->GetName();
        else
            data["materialSave"] = material->SerializeObj();
    }

    data["instancing"] = Instancing;

    return data;
}

void MaterialComponent::UnSerializeObj(const json &j)
{
    if (j.contains("materialName"))
    {
        //TODO: on singleton implementation
    }
    else if (j.contains("materialSave"))
    {
        materialHandle = std::make_shared<Material>();
        material = static_cast<Material*>(materialHandle.get());
        material->UnSerializeObj(j["materialSave"]);
    }

    Instancing = j["instancing"];
}
