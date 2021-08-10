#include "Utils/Primitives.h"

#include "Log.h"
#include <array>
#include "Render/Mesh.h"

Mesh *Utils::CreatePlane(u32 xVertices, u32 yVertices)
{
    if(xVertices == 0 || yVertices == 0)
    {
        logger->Error("Invalid arguments in function: CreatePlane");
        return nullptr;
    }

    auto *vert = new Mesh::Vertex[xVertices * yVertices];
    auto* ind = new u32[(xVertices - 1) * (yVertices - 1) * 6];

    for(u32 i = 0; i < xVertices; ++i)
    {
        u32 di = i * xVertices;
        for(u32 u = 0; u < yVertices; ++u)
        {
            vert[di + u].Position = glm::vec3(i, 0.0f, u);
            vert[di + u].Normal = glm::vec3(0.0f, 1.0f, 0.0f);
            vert[di + u].TexCoords = glm::vec2((float)i / (xVertices - 1), (float)u / (yVertices - 1));
        }
    }

    for(u32 i = 0; i < (xVertices - 1); ++i)
    {
        u32 di = i * (xVertices - 1) * 6;
        u32 ix = i * xVertices;
        u32 ixx = (i + 1) * xVertices;
        for(u32 u = 0; u < (yVertices - 1); ++u)
        {
            //Triangle up
            ind[di + u * 6] = ix + u + 1;
            ind[di + u * 6 + 1] = ixx + u;
            ind[di + u * 6 + 2] = ix + u;

            //Triangle down
            ind[di + u * 6 + 3] = ix + u + 1;
            ind[di + u * 6 + 4] = ixx + u + 1;
            ind[di + u * 6 + 5] = ixx + u;
        }
    }

    auto *mesh = new Mesh(vert, xVertices * yVertices, ind, (xVertices - 1) * (yVertices - 1) * 6);
    mesh->Apply();

    return mesh;
}

Mesh *Utils::CreateCube(float scale)
{
    auto *vert = new Mesh::Vertex[24] {
            //Front
            {glm::vec3(-scale, scale, scale),   glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 1.0f)},
            {glm::vec3(scale, scale, scale),    glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 1.0f)},
            {glm::vec3(scale, -scale, scale),   glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-scale, -scale, scale),  glm::vec3(0.0f, 0.0f, 1.0f),  glm::vec2(0.0f, 0.0f)},
            //Back
            {glm::vec3(scale, scale, -scale),   glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-scale, scale, -scale),  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-scale, -scale, -scale), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(scale, -scale, -scale),  glm::vec3(0.0f, 0.0f, -1.0f), glm::vec2(0.0f, 0.0f)},
            //Top
            {glm::vec3(-scale, scale, -scale),  glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 1.0f)},
            {glm::vec3(scale, scale, -scale),   glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 1.0f)},
            {glm::vec3(scale, scale, scale),    glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-scale, scale, scale),   glm::vec3(0.0f, 1.0f, 0.0f),  glm::vec2(0.0f, 0.0f)},
            //Bottom
            {glm::vec3(scale, -scale, -scale),  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-scale, -scale, -scale), glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-scale, -scale, scale),  glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(scale, -scale, scale),   glm::vec3(0.0f, -1.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            //Left
            {glm::vec3(-scale, scale, -scale),  glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 1.0f)},
            {glm::vec3(-scale, scale, scale),   glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 1.0f)},
            {glm::vec3(-scale, -scale, scale),  glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(1.0f, 0.0f)},
            {glm::vec3(-scale, -scale, -scale), glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec2(0.0f, 0.0f)},
            //Right
            {glm::vec3(scale, scale, scale),    glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 1.0f)},
            {glm::vec3(scale, scale, -scale),   glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 1.0f)},
            {glm::vec3(scale, -scale, -scale),  glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(1.0f, 0.0f)},
            {glm::vec3(scale, -scale, scale),   glm::vec3(1.0f, 0.0f, 0.0f),  glm::vec2(0.0f, 0.0f)}
    };

    auto* ind = new u32[36] {
        0, 3, 1, 1, 3, 2,   // Front
        4, 7, 5, 5, 7, 6,   // Back
        8,11, 9, 9,11,10,   // Top
        12,15,13, 13,15,14, // Bottom
        16,19,17, 17,19,18, // Left
        20,23,21, 21,23,22  // Right
    };

    auto *mesh = new Mesh(vert, 24, ind, 36);
    mesh->Apply();

    return mesh;
}

Mesh *Utils::CreateSphere(u32 detail)
{
    if(detail < 6)
    {
        logger->Error("Invalid arguments in function: CreateSphere, Detail must be > 5");
        return nullptr;
    }

    auto *vert = new Mesh::Vertex[(detail + 1) * (detail + 1)];
    u32 indSize = (detail - 1) * detail * 6;
    auto* ind = new u32[indSize];

    float PI = 3.141592653589f;

    float sectorStep = 2 * PI / detail;
    float stackStep = PI / detail;

    for(u32 i = 0; i <= detail; ++i)
    {
        u32 di = i * (detail + 1);
        float stackAngle = PI / 2 - i * stackStep;
        float xy = glm::cos(stackAngle);
        float z = glm::sin(stackAngle);

        for(u32 j = 0; j <= detail; ++j)
        {
            float sectorAngle = j * sectorStep;

            vert[di + j].Position = glm::vec3(xy * glm::cos(sectorAngle), xy * glm::sin(sectorAngle), z);
            vert[di + j].Normal = vert[di + j].Position;
            vert[di + j].TexCoords = glm::vec2((float)j / detail, (float)i / detail);
        }
    }

    u32 index = 0;
    for(u32 i = 0; i < detail; ++i)
    {
        float k1 = i * (detail + 1);
        float k2 = k1 + detail + 1;

        for(u32 j = 0; j < detail; ++j, ++k1, ++k2)
        {
            if(i != 0)
            {
                ind[index++] = k1;
                ind[index++] = k2;
                ind[index++] = k1 + 1;
            }

            if(i != (detail - 1))
            {
                ind[index++] = k1 + 1;
                ind[index++] = k2;
                ind[index++] = k2 + 1;
            }
        }
    }

    auto *mesh = new Mesh(vert, (detail + 1) * (detail + 1), ind, indSize);
    mesh->Apply();

    return mesh;
}
