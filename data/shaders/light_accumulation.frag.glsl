#version 440 core

in vec4 FragPosLightSpace;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

#include "Shadows.incl"

struct DirectionLight {
    vec4 position;
    vec4 colorAndIntensity;
    vec4 direction;
};

struct PointLight {
    vec4 positionAndIntensity;
    vec4 colorAndRadius;
};

struct SpotLight {
    vec4 positionAndIntensity;
    vec4 colorAndRadius;
    vec4 directionAndCutterAngle;
};

struct VisibleIndex {
	int index;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
	PointLight data[];
} lightBuffer;

layout(std430, binding = 1) readonly buffer SpotLightBuffer {
	SpotLight data[];
} spotLightBuffer;

layout(std430, binding = 2) readonly buffer VisibleLightIndicesBuffer {
	VisibleIndex data[];
} visibleLightIndicesBuffer;

layout(std430, binding = 3) readonly buffer SpotVisibleLightIndicesBuffer {
	VisibleIndex data[];
} spotVisibleLightIndicesBuffer;

uniform DirectionLight dirLight;

uniform vec3 color_diffuse;
uniform float main_specular;
uniform vec3 viewPos;
uniform float ambient = 0.08;
uniform int numberOfTilesX;

layout (location = 0) out vec4 fragColor;
layout (location = 1) out vec4 brightColor;

float attenuate(vec3 lightDirection, float radius)
{
	float cutoff = 0.5;
	float attenuation = dot(lightDirection, lightDirection) / (100.0 * radius);
	attenuation = 1.0 / (attenuation * 15.0 + 1.0);
	attenuation = (attenuation - cutoff) / (1.0 - cutoff);

	return clamp(attenuation, 0.0, 1.0);
}

void main()
{
	ivec2 location = ivec2(gl_FragCoord.xy);
	ivec2 tileID = location / ivec2(16, 16);
	uint index = tileID.y * numberOfTilesX + tileID.x;

	vec4 base_diffuse = vec4(color_diffuse, 1.0);
    vec3 normal = normalize(Normal);
	vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    vec3 viewDirection = normalize(viewPos - FragPos);
    
    {
		vec3 lightDirection = normalize(-dirLight.direction.xyz);
		vec3 halfway = normalize(lightDirection + viewDirection);

		float diffuse = max(dot(lightDirection, normal), 0.0);
		float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		if (diffuse == 0.0)
			specular = 0.0;

		vec3 irradiance = dirLight.colorAndIntensity.rgb * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * dirLight.colorAndIntensity.w;
		color.rgb += irradiance;
    }

	uint offset = index * 1024;
	for (uint i = 0; i < 1024 && visibleLightIndicesBuffer.data[offset + i].index != -1; ++i)
    {
		uint lightIndex = visibleLightIndicesBuffer.data[offset + i].index;
		PointLight light = lightBuffer.data[lightIndex];

		vec3 lightColor = light.colorAndRadius.xyz;
		float lightRadius = light.colorAndRadius.w;

		vec3 lightDirection = light.positionAndIntensity.xyz - FragPos;
		float attenuation = attenuate(lightDirection, lightRadius);

		lightDirection = normalize(lightDirection);
		vec3 halfway = normalize(lightDirection + viewDirection);

		float diffuse = max(dot(lightDirection, normal), 0.0);
		float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		if (diffuse == 0.0)
			specular = 0.0;

		vec3 irradiance = lightColor * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * attenuation * light.positionAndIntensity.w;
		color.rgb += irradiance;
	}
    
	for (uint i = 0; i < 1024 && spotVisibleLightIndicesBuffer.data[offset + i].index != -1; ++i)
    {
		uint lightIndex = spotVisibleLightIndicesBuffer.data[offset + i].index;
		SpotLight light = spotLightBuffer.data[lightIndex];


		vec3 lightColor = light.colorAndRadius.xyz;
		float lightRadius = light.colorAndRadius.w;

		vec3 lightDirection = light.positionAndIntensity.xyz - FragPos;
		float attenuation = attenuate(lightDirection, lightRadius);

		lightDirection = normalize(lightDirection);

        float theta = dot(lightDirection, normalize(-light.directionAndCutterAngle.xyz));
        if(theta <= light.directionAndCutterAngle.w)
            continue;
            
		vec3 halfway = normalize(lightDirection + viewDirection);

		float diffuse = max(dot(lightDirection, normal), 0.0);
		float specular = pow(max(dot(normal, halfway), 0.0), 32.0);

		if (diffuse == 0.0)
			specular = 0.0;

		vec3 irradiance = lightColor * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * attenuation * light.positionAndIntensity.w;
		color.rgb += irradiance;
	}
        
    float shadow = CalculateShadows(normal, FragPos, FragPosLightSpace, dirLight.position);
    
    color.rgb = (1.0 - shadow) * color.rgb;
	color.rgb += base_diffuse.rgb * ambient;

	if (base_diffuse.a <= 0.2)
		discard;
	
	fragColor = color;
    
    float brightness = dot(fragColor.rgb, vec3(0.2126, 0.7152, 0.0722));
    if(brightness > 1.0)
        brightColor = vec4(fragColor.rgb, 1.0);
    else
        brightColor = vec4(0.0, 0.0, 0.0, 1.0);
}