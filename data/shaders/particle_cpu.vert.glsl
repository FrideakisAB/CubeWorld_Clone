#version 440 core

layout (location = 0) in vec4 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in vec4 aSecData;
layout (location = 3) in float aRotation;

uniform mat4 model;
uniform mat4 vp;

out VS_OUT {
    vec4 Color;
    vec4 SecData;
    float Size;
    float Rotation;
} vs_out;

void main()
{
    gl_Position = vp * model * vec4(aPos.xyz, 1.0);

    vs_out.Color = aColor;
    vs_out.SecData = aSecData;
    vs_out.Size = aPos.w;
    vs_out.Rotation = aRotation;
}