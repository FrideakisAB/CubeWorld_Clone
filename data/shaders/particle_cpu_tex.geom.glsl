#version 440 core

layout (points) in;
layout (triangle_strip, max_vertices = 6) out;

uniform float wh;

in VS_OUT {
    vec4 Color;
    vec2 SecData;
    vec4 UV;
    float Size;
    float Rotation;
} gs_in[]; 

out GS_OUT {
    vec4 Color;
    vec2 SecData;
    vec4 UV;
    vec2 UVg;
} gs_out;

const vec2 texCoord[4] = vec2[](vec2(0.0, 1.0), vec2(0.0, 0.0), vec2(1.0, 0.0), vec2(1.0, 1.0));

vec2 Rotate(vec2 a, float Angle)
{
    vec2 ret;

    ret.x = a.x * cos(Angle) - a.y * sin(Angle);
    ret.y = (a.y * cos(Angle) + a.x * sin(Angle)) * wh;

    return ret;
}

void main()
{
    gs_out.Color = gs_in[0].Color;
    gs_out.SecData = gs_in[0].SecData;
    gs_out.UV = gs_in[0].UV;

    vec3 p = gl_in[0].gl_Position.xyz;
    float w = gl_in[0].gl_Position.w;
    float size = gs_in[0].Size;

    gs_out.UVg = texCoord[1];
    gl_Position = vec4(p + vec3(Rotate(vec2(-1.0 * size, -1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.UVg = texCoord[2];
    gl_Position = vec4(p + vec3(Rotate(vec2(1.0 * size, -1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.UVg = texCoord[0];
    gl_Position = vec4(p + vec3(Rotate(vec2(-1.0 * size, 1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.UVg = texCoord[3];
    gl_Position = vec4(p + vec3(Rotate(vec2(1.0 * size, 1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();
    EndPrimitive();
}