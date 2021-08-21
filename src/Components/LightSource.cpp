#include "Components/LightSource.h"

json LightSource::SerializeObj()
{
    json data;

    data["cmpName"] = boost::typeindex::type_id<LightSource>().pretty_name();

    data["type"] = static_cast<u8>(Type);
    data["color"] = {Color.x, Color.y, Color.z};
    data["radius"] = Radius;
    data["intensity"] = Intensity;
    data["cutterOff"] = CutterOff;

    return data;
}

void LightSource::UnSerializeObj(const json &j)
{
    Type = static_cast<LightType>(j["type"]);
    Color = {j["color"][0], j["color"][1], j["color"][2]};
    Radius = j["radius"];
    Intensity = j["intensity"];
    CutterOff = j["cutterOff"];
}
