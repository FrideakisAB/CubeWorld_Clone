#version 440 core

#define HIGH_SHADOWS_COUNT 4
#define MEDIUM_SHADOWS_COUNT 8
#define LOW_SHADOWS_COUNT 48

in vec4 FragPosLightSpace;
in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

struct DirectionLight {
    vec4 position;
    vec4 color;
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

struct Position {
	vec4 pos;
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

layout(std430, binding = 4) readonly buffer ShadowLightPositions {
	Position data[];
} shadowLightPositions;

uniform DirectionLight dirLight;
uniform sampler2D dirShadowMap;

uniform uvec3 pointShadowsCount;
uniform samplerCubeArray pointHighShadowMap;
uniform samplerCubeArray pointMediumShadowMap;
uniform samplerCubeArray pointLowShadowMap;

uniform vec3 color_diffuse;
uniform vec3 viewPos;
uniform float main_specular;
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

float DirectionalShadowCalculation(vec4 fragPosLightSpace, vec4 lightPos)
{
    // perform perspective divide
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    // transform to [0,1] range
    projCoords = projCoords * 0.5 + 0.5;
    // get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
    float closestDepth = texture(dirShadowMap, projCoords.xy).r; 
    // get depth of current fragment from light's perspective
    float currentDepth = projCoords.z;
    // calculate bias (based on depth map resolution and slope)
    vec3 normal = normalize(Normal);
    vec3 lightDir = normalize(lightPos.xyz - FragPos);
    float bias = max(0.05 * (1.0 - dot(normal, lightDir)), 0.005);

    // PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(dirShadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(dirShadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth  ? 1.0 : 0.0;        
        }    
    }
    shadow /= 9.0;
    
    if(projCoords.z > 1.0)
        shadow = 0.0;
        
    return shadow;
}

vec3 gridSamplingDisk[20] = vec3[]
(
   vec3(1, 1,  1), vec3( 1, -1,  1), vec3(-1, -1,  1), vec3(-1, 1,  1), 
   vec3(1, 1, -1), vec3( 1, -1, -1), vec3(-1, -1, -1), vec3(-1, 1, -1),
   vec3(1, 1,  0), vec3( 1, -1,  0), vec3(-1, -1,  0), vec3(-1, 1,  0),
   vec3(1, 0,  1), vec3(-1,  0,  1), vec3( 1,  0, -1), vec3(-1, 0, -1),
   vec3(0, 1,  1), vec3( 0, -1,  1), vec3( 0, -1, -1), vec3( 0, 1, -1)
);

float PointShadowCalculation(vec3 fragPos, uint i, uint pos, samplerCubeArray arr)
{
    vec4 posAndFarPlane = shadowLightPositions.data[pos].pos;
    vec3 fragToLight = fragPos - posAndFarPlane.xyz;
    
    float closestDepth = texture(arr, vec4(fragToLight, i)).r;
    closestDepth *= posAndFarPlane.w;

    float currentDepth = length(fragToLight);

    float bias = 0.05; 
    float shadow = currentDepth -  bias > closestDepth ? 1.0 : 0.0;

    return shadow;
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

		vec3 irradiance = dirLight.color.rgb * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * dirLight.color.w;
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

		vec3 irradiance = lightColor * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * attenuation;
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

		vec3 irradiance = lightColor * ((base_diffuse.rgb * diffuse) + vec3(specular * main_specular)) * attenuation;
		color.rgb += irradiance;
	}
        
    float shadow = DirectionalShadowCalculation(FragPosLightSpace, dirLight.position);
    for (uint i = 0; i < pointShadowsCount.x; ++i)
        shadow += PointShadowCalculation(FragPos, i, i, pointHighShadowMap);
    for (uint i = 0; i < pointShadowsCount.y; ++i)
        shadow += PointShadowCalculation(FragPos, i, pointShadowsCount.x + i, pointMediumShadowMap);
    for (uint i = 0; i < pointShadowsCount.z; ++i)
        shadow += PointShadowCalculation(FragPos, i, pointShadowsCount.x + pointShadowsCount.y + i, pointLowShadowMap);
    
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