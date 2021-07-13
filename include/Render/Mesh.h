#ifndef MESH_H
#define MESH_H

#include "Utils/glm.h"
#include "Render/Render.h"

class Mesh : public DrawObject {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    static const u8 VertexAttribCount;
    static const Attrib VertexAttrib[3];

protected:
    void SubmitData(DrawData &drawData) override;

private:
    DrawType drawType = DrawType::Static;
    Vertex *vertices = nullptr;
    u32 vertCount = 0;
    u32 *indices = nullptr;
    u32 indCount = 0;
    u32 VAO = 0, VBO = 0, EBO = 0;
    bool needCreateVertices = true;
    bool needCreateIndices = true;

    void createMesh();

public:
    Mesh() = default;
    Mesh(Vertex *vertices, u32 vertCount, u32 *indices, u32 indCount, DrawType drawType=DrawType::Static);
    Mesh(const Mesh &mesh);
    Mesh(Mesh &&mesh) noexcept;
    ~Mesh();

    Mesh &operator=(const Mesh &mesh);
    Mesh &operator=(Mesh &&mesh) noexcept;

    [[nodiscard]] Vertex *GetVertices() const noexcept { return vertices; }
    [[nodiscard]] u32 GetVertexCount() const noexcept { return vertCount; }
    [[nodiscard]] u32 *GetIndices() const noexcept { return indices; }
    [[nodiscard]] u32 GetIndicesCount() const noexcept { return indCount; }

    void SetVertices(Vertex *vertices, u32 vertCount) noexcept;
    void SetIndices(u32 *indices, u32 indCount) noexcept;

    void CalculateNormals();
};

#endif
