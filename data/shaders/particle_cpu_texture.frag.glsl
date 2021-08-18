#version 440 core
#extension GL_ARB_bindless_texture : enable

in GS_OUT {
    vec4 Color;
    vec2 SecData;
    vec4 UV;
    float Pos;
} fs_in;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

layout(bindless_sampler) uniform sampler2D texture_diffuse;

const vec2 texCoord[4] = vec2[](vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0));

void main()
{
    if(fs_in.SecData.x == 0.0)
        discard;
        
    vec2 texCoordRes = vec2(0.0, 0.0);
    texCoordRes.x = (fs_in.UV.x + texCoord[int(fs_in.Pos)].x) * fs_in.UV.w;
    texCoordRes.y = (fs_in.UV.y + texCoord[int(fs_in.Pos)].y) * fs_in.UV.z;
	
    fragColor = fs_in.Color * texture(texture_diffuse, texCoordRes);
    brightColor = vec4(fragColor.rgb * fs_in.SecData.y, 1.0);
}