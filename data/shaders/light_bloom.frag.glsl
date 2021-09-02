#version 440 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirectionLight {
    vec4 position;
    vec4 color;
    vec4 direction;
};

uniform vec3 color_light;
uniform vec3 viewPos;
uniform float ambient = 0.08;
uniform int numberOfTilesX;
uniform DirectionLight dirLight;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

void main()
{
	fragColor = vec4(color_light, 1.0);
    //float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    // For a more noticeable effect
    brightColor = vec4(fragColor.rgb, 1.0);
    //if(brightness > 1.0)
    //  brightColor = vec4(fragColor.rgb, 1.0);
	//else
	//  brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}