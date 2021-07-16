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
};

class MaterialComponent : public ECS::Component<MaterialComponent> {
public:
    AssetsHandle Material;
    bool Instancing = true;
};

#endif
