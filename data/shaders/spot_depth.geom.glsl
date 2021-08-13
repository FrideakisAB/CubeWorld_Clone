#version 440 core

layout (triangles) in;
layout (triangle_strip, max_vertices=3) out;

uniform int layer;

out vec4 fragPos;

void main()
{
    gl_Layer = layer;
    
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();
    EndPrimitive();
}