#version 440 core

layout (location = 0) in vec4 aPosAndSize;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec2 aSecData;
layout (location = 3) in vec4 aUV;
layout (location = 4) in float aRotation;

uniform mat4 model;
uniform mat4 vp;

out VS_OUT {
    vec4 Color;
    vec2 SecData;
    vec4 UV;
    float Size;
    float Rotation;
} vs_out;

void main()
{
    gl_Position = vp * model * vec4(aPosAndSize.xyz, 1.0);

    vs_out.Color = aColor;
    vs_out.SecData = aSecData;
    vs_out.UV = aUV;
    vs_out.Size = aPosAndSize.w;
    vs_out.Rotation = aRotation;
}