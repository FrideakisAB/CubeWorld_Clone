#ifndef MATERIAL_H
#define MATERIAL_H

#include "ECS/ECS.h"
#include "Utils/glm.h"
#include "Render/Shader.h"

class Material : public ECS::Component<Material> {
private:
    // Shader handle

public:
    Material() = default;
};

#endif
