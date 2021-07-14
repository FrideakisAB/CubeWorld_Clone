#include "Components/LightSource.h"

void LightSource::SetLightType(LightType lightType)
{
    update = true;
    type = lightType;
}

void LightSource::SetColor(glm::vec3 color)
{
    update = true;
    this->color = color;
}

void LightSource::SetRadius(float radius)
{
    update = true;
    this->radius = radius;
}

void LightSource::SetIntensity(float intensity)
{
    update = true;
    this->intensity = intensity;
}

void LightSource::SetCutterOff(float cutterOff)
{
    update = true;
    this->cuttOff = cutterOff;
}
