#include "Editor/Render/LinesMesh.h"

#include "Render/GLUtils.h"

const u8 VertexAttribCount = 3;
const Attrib LinesMesh::VertexAttrib[2] = {
        // Vertex positions
        {3, sizeof(LinesMesh::Vertex), 0, ValueType::Float, false},
        // Vertex color
        {3, sizeof(LinesMesh::Vertex), (u32)offsetof(LinesMesh::Vertex, Color), ValueType::Float, false},
};

void LinesMesh::SubmitData(DrawData &drawData)
{
    if (drawData.VAO != 0)
    {
        if (needCreateVertices)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(Vertex), &vertices[0], Utils::GetDrawTypeGL(MeshDrawType));
            needCreateVertices = false;
            apply = false;
        }
        else if (vertices != nullptr && apply)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            auto *verticesGPU = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (u32 i = 0; i < vertCount; ++i)
                verticesGPU[i] = vertices[i];
            glUnmapBuffer(GL_ARRAY_BUFFER);
            apply = false;
        }

        if (isStatic)
        {
            delete[] vertices;
            vertices = nullptr;
        }

        drawData.Count = vertPos;
    }
    else
    {
        createMesh();
        drawData.VAO = VAO;
        drawData.Mode = DrawMode::Lines;
        drawData.Primitive = Primitive::Lines;
        drawData.Count = vertPos;
        drawData.Arrays = true;
    }
}

void LinesMesh::createMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(Vertex), &vertices[0], Utils::GetDrawTypeGL(MeshDrawType));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Color));

    glBindVertexArray(0);
}

LinesMesh::LinesMesh()
{
    needCreateVertices = true;
    isStatic = false;
    apply = false;
}

LinesMesh::LinesMesh(LinesMesh::Vertex *vertices, u32 vertCount, DrawType drawType)
    : vertices(vertices), vertCount(vertCount), vertPos(vertCount), MeshDrawType(drawType)
{
    needCreateVertices = true;
    isStatic = false;
    apply = false;
}

LinesMesh::LinesMesh(const LinesMesh &mesh)
{
    needCreateVertices = true;
    isStatic = false;
    apply = false;

    vertCount = mesh.vertCount;
    vertPos = mesh.vertPos;
    vertices = new Vertex[mesh.vertCount];
    std::copy(mesh.vertices, mesh.vertices + mesh.vertCount, vertices);
}

LinesMesh::LinesMesh(LinesMesh &&mesh) noexcept
    : DrawObject(std::move(mesh))
{
    if (this == &mesh)
        return;

    std::swap(vertices, mesh.vertices);
    std::swap(vertCount, mesh.vertCount);
    std::swap(vertPos, mesh.vertPos);

    needCreateVertices = mesh.needCreateVertices;
    isStatic = mesh.isStatic;
    apply = mesh.apply;
}

LinesMesh::~LinesMesh()
{
    delete[] vertices;
}

LinesMesh &LinesMesh::operator=(const LinesMesh &mesh)
{
    if (this == &mesh)
        return *this;

    delete[] vertices;
    vertCount = mesh.vertCount;
    vertPos = mesh.vertPos;
    vertices = new Vertex[mesh.vertCount];
    std::copy(mesh.vertices, mesh.vertices + mesh.vertCount, vertices);

    return *this;
}

LinesMesh &LinesMesh::operator=(LinesMesh &&mesh) noexcept
{
    if (this == &mesh)
        return *this;

    std::swap(vertices, mesh.vertices);
    std::swap(vertCount, mesh.vertCount);
    std::swap(vertPos, mesh.vertPos);

    return *this;
}

void LinesMesh::SetVertices(LinesMesh::Vertex *vertices, u32 vertCount) noexcept
{
    if (this->vertCount != vertCount)
        needCreateVertices = true;
    delete[] this->vertices;

    this->vertices = vertices;
    this->vertCount = vertCount;
    vertPos = vertCount;
}

void LinesMesh::Apply() noexcept
{
    apply = true;
}

void LinesMesh::SetStatic(bool active) noexcept
{
    isStatic = active;
}

void LinesMesh::Recreate() noexcept
{
    needCreateVertices = true;
}

void LinesMesh::ReserveVertex(u32 count)
{
    auto *verticesNew = new Vertex[vertCount + count * 2];
    if (vertices != nullptr)
    {
        std::copy(vertices, vertices + (vertCount - 1), verticesNew);
        delete[] vertices;
    }
    vertCount += count * 2;
    vertices = verticesNew;
}

void LinesMesh::ResetPrimitives()
{
    vertPos = 0;
}

void LinesMesh::AddLine(Segment segment)
{
    if (vertPos >= vertCount / 2)
        ReserveVertex(1);

    vertices[vertPos * 2] = { segment.StartPosition, segment.Color };
    vertices[vertPos * 2 + 1] = { segment.EndPosition, segment.Color };

    ++vertPos;
}

void LinesMesh::AddCircle(const glm::vec3 &center, f32 radius, glm::vec3 color, u32 lineCount, const glm::mat3 &mat)
{
    if (vertPos + lineCount - 1 >= vertCount / 2)
        ReserveVertex(lineCount);

    const f32 PI = 3.141592653589f;

    f32 circleStep = 2 * PI / lineCount;

    for (u32 i = 0; i < lineCount; ++i)
    {
        vertices[vertPos * 2].Position = center + glm::vec3(radius * glm::cos(i * circleStep), radius * glm::sin(i * circleStep), 0.0f) * mat;
        vertices[vertPos * 2].Color = color;
        if(i != (lineCount - 1))
            vertices[vertPos * 2 + 1].Position = center + glm::vec3(radius * glm::cos((i + 1) * circleStep), radius * glm::sin((i + 1) * circleStep), 0.0f) * mat;
        else
            vertices[vertPos * 2 + 1].Position = center + glm::vec3(radius * glm::cos(0), radius * glm::sin(0), 0.0f) * mat;
        vertices[vertPos * 2 + 1].Color = color;

        ++vertPos;
    }
}

void LinesMesh::AddSphere(const glm::vec3 &center, f32 radius, glm::vec3 color, u32 lineCount)
{
    if (vertPos + lineCount * 3 - 1 >= vertCount / 2)
        ReserveVertex(lineCount * 3);

    glm::mat4 trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    AddCircle(center, radius, color, lineCount, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
    trans = glm::mat4(1.0f);
    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    AddCircle(center, radius, color, lineCount, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
    AddCircle(center, radius, color, lineCount);
}

void LinesMesh::AddPlane(const glm::vec3 &center, f32 distance, glm::vec3 color, const glm::mat3 &mat)
{
    if (vertPos + 3 >= vertCount / 2)
        ReserveVertex(4);

    vertices[vertPos * 2].Position = center + glm::vec3(-distance, distance, 0.0f) * mat;
    vertices[vertPos * 2].Color = color;
    vertices[vertPos * 2 + 1].Position = center + glm::vec3(distance, distance, 0.0f) * mat;
    vertices[vertPos * 2 + 1].Color = color;
    ++vertPos;
    vertices[vertPos * 2].Position = center + glm::vec3(-distance, -distance, 0.0f) * mat;
    vertices[vertPos * 2].Color = color;
    vertices[vertPos * 2 + 1].Position = center + glm::vec3(distance, -distance, 0.0f) * mat;
    vertices[vertPos * 2 + 1].Color = color;
    ++vertPos;
    vertices[vertPos * 2].Position = center + glm::vec3(-distance, distance, 0.0f) * mat;
    vertices[vertPos * 2].Color = color;
    vertices[vertPos * 2 + 1].Position = center + glm::vec3(-distance, -distance, 0.0f) * mat;
    vertices[vertPos * 2 + 1].Color = color;
    ++vertPos;
    vertices[vertPos * 2].Position = center + glm::vec3(distance, distance, 0.0f) * mat;
    vertices[vertPos * 2].Color = color;
    vertices[vertPos * 2 + 1].Position = center + glm::vec3(distance, -distance, 0.0f) * mat;
    vertices[vertPos * 2 + 1].Color = color;
    ++vertPos;
}

void LinesMesh::AddCurve(const glm::vec3 *points, u32 count, glm::vec3 color, const glm::mat3 &mat)
{
    if (vertPos + count * 2 - 1 >= vertCount / 2)
        ReserveVertex(count * 2);

    for (u32 i = 1; i < count; ++i)
    {
        vertices[vertPos * 2].Position = points[i - 1] * mat;
        vertices[vertPos * 2].Color = color;
        vertices[vertPos * 2 + 1].Position = points[i] * mat;
        vertices[vertPos * 2 + 1].Color = color;
        ++vertPos;
    }
}

void LinesMesh::AddArc(const glm::vec3 &center, f32 radius, f32 angle, glm::vec3 color, u32 lineCount, const glm::mat3 &mat)
{
    if (vertPos + lineCount - 1 >= vertCount / 2)
        ReserveVertex(lineCount);

    f32 circleStep = glm::radians(angle) / lineCount;

    for(u32 i = 0; i < lineCount; ++i)
    {
        vertices[vertPos * 2].Position = center + glm::vec3(radius * glm::cos(i * circleStep), radius * glm::sin(i * circleStep), 0.0f) * mat;
        vertices[vertPos * 2].Color = color;
        vertices[vertPos * 2 + 1].Position = center + glm::vec3(radius * glm::cos((i + 1) * circleStep), radius * glm::sin((i + 1) * circleStep), 0.0f) * mat;
        vertices[vertPos * 2 + 1].Color = color;
        ++vertPos;
    }
}
