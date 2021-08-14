#include "Components/LightSource.h"

void LightSource::SetLightType(LightType lightType)
{
    type = lightType;
}

void LightSource::SetColor(glm::vec3 color)
{
    this->color = color;
}

void LightSource::SetRadius(float radius)
{
    this->radius = radius;
}

void LightSource::SetIntensity(float intensity)
{
    this->intensity = intensity;
}

void LightSource::SetCutterOff(float cutterOff)
{
    this->cuttOff = cutterOff;
}
