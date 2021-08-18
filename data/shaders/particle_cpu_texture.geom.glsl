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
    float Pos;
} gs_out;

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

    gs_out.Pos = 1.0;
    gl_Position = vec4(p + vec3(Rotate(vec2(-1.0 * size, -1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.Pos = 2.0;
    gl_Position = vec4(p + vec3(Rotate(vec2(1.0 * size, -1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.Pos = 0.0;
    gl_Position = vec4(p + vec3(Rotate(vec2(-1.0 * size, 1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();

    gs_out.Pos = 3.0;
    gl_Position = vec4(p + vec3(Rotate(vec2(1.0 * size, 1.0 * size), gs_in[0].Rotation).xy, 0.0), w);
    EmitVertex();
    EndPrimitive();
}