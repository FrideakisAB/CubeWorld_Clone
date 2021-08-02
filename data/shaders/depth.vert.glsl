#version 440 core

layout (location = 0) in vec3 position;

uniform mat4 vp;
uniform mat4 model;

void main()
{
    gl_Position = vp * model * vec4(position, 1.0);
}