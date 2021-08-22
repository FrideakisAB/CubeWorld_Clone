#ifndef MESH_H
#define MESH_H

#include "Utils/glm.h"
#include "Assets/IAsset.h"
#include "Render/Render.h"
#include "boost/type_index.hpp"

class Mesh final : public DrawObject, public IAsset {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Normal;
        glm::vec2 TexCoords;
    };

    static const u8 VertexAttribCount;
    static const Attrib VertexAttrib[3];

protected:
    void SubmitData(DrawData &drawData) final;

private:
    Vertex *vertices = nullptr;
    u32 vertCount = 0;
    u32 *indices = nullptr;
    u32 indCount = 0;
    u32 VAO = 0, VBO = 0, EBO = 0;
    u8 needCreateVertices : 1;
    u8 needCreateIndices : 1;
    u8 isStatic : 1;
    u8 apply : 1;
    u8 reserved : 4;

    void createMesh();

public:
    Mesh();
    Mesh(Vertex *vertices, u32 vertCount, u32 *indices, u32 indCount, DrawType drawType=DrawType::Static);
    Mesh(const Mesh &mesh);
    Mesh(Mesh &&mesh) noexcept;
    ~Mesh() final;

    Mesh &operator=(const Mesh &mesh);
    Mesh &operator=(Mesh &&mesh) noexcept;

    [[nodiscard]] Vertex *GetVertices() const noexcept { return vertices; }
    [[nodiscard]] u32 GetVertexCount() const noexcept { return vertCount; }
    [[nodiscard]] u32 *GetIndices() const noexcept { return indices; }
    [[nodiscard]] u32 GetIndicesCount() const noexcept { return indCount; }

    void SetVertices(Vertex *vertices, u32 vertCount) noexcept;
    void SetIndices(u32 *indices, u32 indCount) noexcept;

    [[nodiscard]] bool IsStatic() const noexcept { return isStatic; }
    void SetStatic(bool active) noexcept;

    void CalculateNormals();

    [[nodiscard]] size_t GetTypeID() const noexcept final
    {
        return boost::typeindex::type_id<Mesh>().hash_code();
    }

    [[nodiscard]] IAsset* Clone() const final;

    void Apply() noexcept;
    void Recreate() noexcept;

    [[nodiscard]] json SerializeObj() final;
    void UnSerializeObj(const json& j) final;

    void SerializeBin(std::ofstream &file) final;
    void UnSerializeBin(std::ifstream &file) final;

    DrawType MeshDrawType = DrawType::Static;
};

#endif
