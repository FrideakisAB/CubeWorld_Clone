#include "Render/Mesh.h"

#include <map>
#include "Render/GLUtils.h"

const u8 VertexAttribCount = 3;
const Attrib Mesh::VertexAttrib[3] = {
        // Vertex positions
        {3, sizeof(Mesh::Vertex), 0, ValueType::Float, false},
        // Vertex normals
        {3, sizeof(Mesh::Vertex), (u32)offsetof(Mesh::Vertex, Normal), ValueType::Float, false},
        // Vertex texture coords
        {2, sizeof(Mesh::Vertex), (u32)offsetof(Mesh::Vertex, TexCoords), ValueType::Float, false}
};

void Mesh::SubmitData(DrawData &drawData)
{
    if (drawData.VAO != 0)
    {
        if (needCreateVertices)
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(Vertex), &vertices[0], Utils::GetDrawTypeGL(drawType));
        }
        else
        {
            glBindBuffer(GL_ARRAY_BUFFER, VBO);
            auto *verts = static_cast<Vertex*>(glMapBuffer(GL_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (u32 i = 0; i < vertCount; ++i)
                verts[i] = vertices[i];
            glUnmapBuffer(GL_ARRAY_BUFFER);
        }

        if (needCreateIndices)
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(u32), &indices[0], Utils::GetDrawTypeGL(drawType));
        }
        else
        {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
            auto *inds = static_cast<u32*>(glMapBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_WRITE_ONLY));
            for (u32 i = 0; i < indCount; ++i)
                inds[i] = indices[i];
            glUnmapBuffer(GL_ELEMENT_ARRAY_BUFFER);
        }

        drawData.Count = indCount;
    }
    else
    {
        createMesh();
        drawData.VAO = VAO;
        drawData.primitive = Primitive::Triangles;
        drawData.Count = indCount;
    }
}

Mesh::Mesh(Mesh::Vertex *vertices, u32 vertCount, u32 *indices, u32 indCount, DrawType drawType) :
    vertices(vertices), vertCount(vertCount), indices(indices), indCount(indCount), drawType(drawType)
{}

Mesh::Mesh(const Mesh &mesh)
 : DrawObject(mesh)
{
    vertCount = mesh.vertCount;
    vertices = new Vertex[mesh.vertCount];
    std::copy(mesh.vertices, mesh.vertices + mesh.vertCount, vertices);

    indCount = mesh.indCount;
    indices = new u32[mesh.indCount];
    std::copy(mesh.indices, mesh.indices + mesh.indCount, indices);
}

Mesh::Mesh(Mesh &&mesh) noexcept
{
    if(this == &mesh)
        return;

    std::swap(vertices, mesh.vertices);
    std::swap(indices, mesh.indices);

    std::swap(vertCount, mesh.vertCount);
    std::swap(indCount, mesh.indCount);
}

Mesh::~Mesh()
{
    delete[] vertices;
    delete[] indices;

    auto deleter = [=](){
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);
    };
    ReleaseHandle(deleter);
}

Mesh &Mesh::operator=(const Mesh &mesh)
{
    if(this == &mesh)
        return *this;

    delete[] vertices;
    vertCount = mesh.vertCount;
    vertices = new Vertex[mesh.vertCount];
    std::copy(mesh.vertices, mesh.vertices + mesh.vertCount, vertices);

    delete[] this->indices;
    indCount = mesh.indCount;
    indices = new u32[mesh.indCount];
    std::copy(mesh.indices, mesh.indices + mesh.indCount, indices);

    return *this;
}

Mesh &Mesh::operator=(Mesh &&mesh) noexcept
{
    if(this == &mesh)
        return *this;

    std::swap(vertices, mesh.vertices);
    std::swap(indices, mesh.indices);

    std::swap(vertCount, mesh.vertCount);
    std::swap(indCount, mesh.indCount);

    return *this;
}

void Mesh::SetVertices(Mesh::Vertex *vertices, u32 vertCount) noexcept
{
    if (this->vertCount != vertCount)
        needCreateVertices = true;
    delete[] this->vertices;

    this->vertices = vertices;
    this->vertCount = vertCount;
}

void Mesh::SetIndices(u32 *indices, u32 indCount) noexcept
{
    if (this->indCount != indCount)
        needCreateIndices = true;
    delete[] this->indices;

    this->indices = indices;
    this->indCount = indCount;
}

float sqr(float x) { return x * x; }

void Mesh::CalculateNormals()
{
    std::map<u32, glm::vec3> trNormals;
    std::vector<u32> nCount;
    for(u32 i = 0; i < vertCount; ++i)
    {
        nCount.push_back(0);
        for(u32 j = 0; j < indCount; j+=3)
            if(indices[j] == i || indices[j+1] == i || indices[j+2] == i)
            {
                if(trNormals.find(j/3) == trNormals.end())
                {
                    glm::vec3 norm;

                    glm::vec3 v1 = vertices[indices[j]].Position - vertices[indices[j+1]].Position;
                    glm::vec3 v2 = vertices[indices[j+1]].Position - vertices[indices[j+2]].Position;

                    float wrki = sqrt(sqr(v1.y*v2.z - v1.z * v2.y) + sqr(v1.z * v2.x - v1.x * v2.z) + sqr(v1.x * v2.y - v1.y * v2.x));
                    norm.x = (v1.y * v2.z - v1.z * v2.y) / wrki;
                    norm.y = (v1.z * v2.x - v1.x * v2.z) / wrki;
                    norm.z = (v1.x * v2.y - v1.y * v2.x) / wrki;

                    trNormals[j/3] = norm;
                }

                vertices[i].Normal += trNormals[j/3];
                ++nCount[i];
            }
    }

    for(u32 i = 0; i < vertCount; ++i)
        if(nCount[i] != 0)
        {
            vertices[i].Normal /= (float)nCount[i];
            vertices[i].Normal = glm::normalize(vertices[i].Normal);
        }
}

void Mesh::createMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertCount * sizeof(Vertex), &vertices[0], Utils::GetDrawTypeGL(drawType));

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indCount * sizeof(u32), &indices[0], Utils::GetDrawTypeGL(drawType));

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)nullptr);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));

    glBindVertexArray(0);
}
