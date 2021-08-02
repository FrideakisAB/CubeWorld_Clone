#version 440 core

struct SpotLight {
    vec4 positionAndIntensity;
    vec4 colorAndRadius;
    vec4 directionAndCutterAngle;
};

struct VisibleIndex {
    int index;
};

layout(std430, binding = 0) readonly buffer LightBuffer {
    SpotLight data[];
} lightBuffer;

layout(std430, binding = 1) writeonly buffer VisibleLightIndicesBuffer {
    VisibleIndex data[];
} visibleLightIndicesBuffer;

uniform sampler2D depthMap;
uniform mat4 view;
uniform mat4 projection;
uniform ivec2 screenSize;
uniform int lightCount;

shared uint minDepthInt;
shared uint maxDepthInt;
shared uint visibleLightCount;
shared vec4 frustumPlanes[6];
shared int visibleLightIndices[1024];
shared mat4 viewProjection;

#define TILE_SIZE 16
layout(local_size_x = TILE_SIZE, local_size_y = TILE_SIZE, local_size_z = 1) in;
void main()
{
    ivec2 location = ivec2(gl_GlobalInvocationID.xy);
    ivec2 itemID = ivec2(gl_LocalInvocationID.xy);
    ivec2 tileID = ivec2(gl_WorkGroupID.xy);
    ivec2 tileNumber = ivec2(gl_NumWorkGroups.xy);
    uint index = tileID.y * tileNumber.x + tileID.x;

    if (gl_LocalInvocationIndex == 0)
    {
        minDepthInt = 0xFFFFFFFF;
        maxDepthInt = 0;
        visibleLightCount = 0;
        viewProjection = projection * view;
    }

    barrier();

    float maxDepth, minDepth;
    vec2 text = vec2(location) / screenSize;
    float depth = texture(depthMap, text).r;
    depth = (0.5 * projection[3][2]) / (depth + 0.5 * projection[2][2] - 0.5);

    uint depthInt = floatBitsToUint(depth);
    atomicMin(minDepthInt, depthInt);
    atomicMax(maxDepthInt, depthInt);

    barrier();

    if (gl_LocalInvocationIndex == 0)
    {
        minDepth = uintBitsToFloat(minDepthInt);
        maxDepth = uintBitsToFloat(maxDepthInt);

        vec2 negativeStep = (2.0 * vec2(tileID)) / vec2(tileNumber);
        vec2 positiveStep = (2.0 * vec2(tileID + ivec2(1, 1))) / vec2(tileNumber);

        frustumPlanes[0] = vec4(1.0, 0.0, 0.0, 1.0 - negativeStep.x); // Left
        frustumPlanes[1] = vec4(-1.0, 0.0, 0.0, -1.0 + positiveStep.x); // Right
        frustumPlanes[2] = vec4(0.0, 1.0, 0.0, 1.0 - negativeStep.y); // Bottom
        frustumPlanes[3] = vec4(0.0, -1.0, 0.0, -1.0 + positiveStep.y); // Top
        frustumPlanes[4] = vec4(0.0, 0.0, -1.0, -minDepth); // Near
        frustumPlanes[5] = vec4(0.0, 0.0, 1.0, maxDepth); // Far

        for (uint i = 0; i < 4; ++i)
        {
            frustumPlanes[i] *= viewProjection;
            frustumPlanes[i] /= length(frustumPlanes[i].xyz);
		}

        frustumPlanes[4] *= view;
        frustumPlanes[4] /= length(frustumPlanes[4].xyz);
        frustumPlanes[5] *= view;
        frustumPlanes[5] /= length(frustumPlanes[5].xyz);
    }

    barrier();

    uint threadCount = TILE_SIZE * TILE_SIZE;
    uint passCount = (lightCount + threadCount - 1) / threadCount;
    for (uint i = 0; i < passCount; ++i)
    {
        uint lightIndex = i * threadCount + gl_LocalInvocationIndex;
        if (lightIndex >= lightCount)
            break;

        vec4 position = vec4(lightBuffer.data[lightIndex].positionAndIntensity.xyz, 1.0);
        float radius = lightBuffer.data[lightIndex].colorAndRadius.w;

        float distance = 0.0;
        for (uint j = 0; j < 6; ++j)
        {
            distance = dot(position, frustumPlanes[j]) + radius;

            if (distance <= 0.0)
                break;
        }

        if (distance > 0.0)
        {
            uint offset = atomicAdd(visibleLightCount, 1);
            visibleLightIndices[offset] = int(lightIndex);
        }
    }

    barrier();

    if (gl_LocalInvocationIndex == 0)
    {
        uint offset = index * 1024;
        for (uint i = 0; i < visibleLightCount; ++i)
            visibleLightIndicesBuffer.data[offset + i].index = visibleLightIndices[i];

        if (visibleLightCount != 1024)
            visibleLightIndicesBuffer.data[offset + visibleLightCount].index = -1;
    }
}