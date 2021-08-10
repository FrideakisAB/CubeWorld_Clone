#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "Platform.h"
class Mesh;

namespace Utils {
    Mesh *CreatePlane(u32 xVertices = 16, u32 yVertices = 16);
    Mesh *CreateCube(float scale = 1.0f);
    Mesh *CreateSphere(u32 detail);
}

#endif
