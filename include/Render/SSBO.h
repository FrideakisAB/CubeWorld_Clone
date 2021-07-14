#ifndef SSBO_H
#define SSBO_H

#include "Render/GLUtils.h"

template<typename T>
class SSBO {
    size_t arrayLength;
    T *data = nullptr;
    GLuint buffId;
    GLuint buffType;
    GLuint loc;

public:
    SSBO() :
        arrayLength(0), buffType(0), loc(0), buffId(0)
    {
        glGenBuffers(1, &buffId);
    }

    explicit SSBO(size_t elementsCount, T *sData = nullptr, GLuint bufferType = GL_SHADER_STORAGE_BUFFER, GLuint memType = GL_DYNAMIC_DRAW) :
        arrayLength(elementsCount), buffType(bufferType), loc(0), buffId(0)
    {
        glGenBuffers(1, &buffId);
        glBindBufferBase(bufferType, 0, buffId);
        glBufferData(bufferType, sizeof(T) * elementsCount, sData, memType);
    }

    ~SSBO()
    {
        glBindBuffer(buffType, 0);
        glDeleteBuffers(1, &buffId);
    }

    void UploadData(size_t elementsCount, T *sData = nullptr, GLuint bufferType = GL_SHADER_STORAGE_BUFFER, GLuint memType = GL_DYNAMIC_DRAW)
    {
        arrayLength = elementsCount; buffType = bufferType; loc = 0;
        glBindBufferBase(bufferType, 0, buffId);
        glBufferData(bufferType, sizeof(T) * elementsCount, sData, memType);
    }

    void UpdateData(size_t elementsCount, T *sData, size_t offset = 0, GLuint bufferType = GL_SHADER_STORAGE_BUFFER)
    {
        glBindBufferBase(bufferType, 0, buffId);
        glBufferSubData(bufferType, sizeof(T) * offset, sizeof(T) * elementsCount, sData);
    }

    void Open()
    {
        glBindBuffer(buffType, buffId);
        data = glMapBuffer(buffType, GL_READ_WRITE);
    }

    void Close()
    {
        glBindBuffer(buffType, buffId);
        glUnmapBuffer(buffType);
        glBindBuffer(buffType, 0);
        data = nullptr;
    }

    [[nodiscard]] T *GetData() const noexcept { return data; }
    [[nodiscard]] GLuint GetBuffId() const noexcept { return buffId; }
    [[nodiscard]] size_t GetCount() const noexcept { return arrayLength; }
    [[nodiscard]] size_t GetBufferSize() const noexcept { return arrayLength * sizeof(T); }
    [[nodiscard]] bool IsValid() const noexcept { return arrayLength != 0; }

    void Bind(GLuint location = 0)
    {
        glBindBufferBase(buffType, location, buffId);
        loc = location;
    }

    void BindAs(GLuint buff_type, GLuint location = 0)
    {
        glBindBufferBase(buff_type, location, buffId);
        loc = location;
    }

    void BindAsAttrib(GLuint location = 0)
    {
        glBindBuffer(GL_ARRAY_BUFFER, buffId);
        glVertexAttribPointer(location, sizeof(T) / sizeof(float), GL_FLOAT, false, 0, nullptr);
        glEnableVertexAttribArray(location);
        loc = location;
    }
};

#endif
