#ifndef LINESMESH_H
#define LINESMESH_H

#include "Utils/glm.h"
#include "Render/Render.h"
#include "boost/type_index.hpp"

struct Segment {
    glm::vec3 StartPosition;
    glm::vec3 EndPosition;
    glm::vec3 Color;
};

class LinesMesh final : public DrawObject {
public:
    struct Vertex {
        glm::vec3 Position;
        glm::vec3 Color;
    };

    static const u8 VertexAttribCount;
    static const Attrib VertexAttrib[2];

protected:
    void SubmitData(DrawData &drawData) final;

private:
    Vertex *vertices = nullptr;
    u32 vertCount = 0;
    u32 vertPos = 0;
    u32 VAO = 0, VBO = 0;
    u8 needCreateVertices : 1;
    u8 isStatic : 1;
    u8 apply : 1;
    u8 reserved : 5;

    void createMesh();

public:
    LinesMesh();
    LinesMesh(Vertex *vertices, u32 vertCount, DrawType drawType=DrawType::Static);
    LinesMesh(const LinesMesh &mesh);
    LinesMesh(LinesMesh &&mesh) noexcept;
    ~LinesMesh();

    LinesMesh &operator=(const LinesMesh &mesh);
    LinesMesh &operator=(LinesMesh &&mesh) noexcept;

    [[nodiscard]] Vertex *GetVertices() const noexcept { return vertices; }
    [[nodiscard]] u32 GetVertexCount() const noexcept { return vertCount; }

    void SetVertices(Vertex *vertices, u32 vertCount) noexcept;

    [[nodiscard]] bool IsStatic() const noexcept { return isStatic; }
    void SetStatic(bool active) noexcept;

    void Apply() noexcept;
    void Recreate() noexcept;

    void ReserveVertex(u32 count);
    void ResetPrimitives();
    void AddLine(Segment segment);
    void AddCircle(const glm::vec3 &center, f32 radius, glm::vec3 color, u32 lineCount=8, const glm::mat3 &mat=glm::mat3(1.0f));
    void AddSphere(const glm::vec3 &center, f32 radius, glm::vec3 color, u32 lineCount=8);
    void AddPlane(const glm::vec3 &center, f32 distance, glm::vec3 color, const glm::mat3 &mat=glm::mat3(1.0f));
    void AddCurve(const glm::vec3 *points, u32 count, glm::vec3 color, const glm::mat3 &mat=glm::mat3(1.0f));
    void AddArc(const glm::vec3 &center, f32 radius, f32 angle, glm::vec3 color, u32 lineCount=8, const glm::mat3 &mat=glm::mat3(1.0f));

    DrawType MeshDrawType = DrawType::Static;
};

#endif
