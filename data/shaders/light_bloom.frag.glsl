#version 440 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

uniform vec3 lightColor;
uniform vec3 viewPos;
uniform float ambient = 0.08;
uniform int numberOfTilesX;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
	fragColor = vec4(lightColor, 1.0);
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
	else
		brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}