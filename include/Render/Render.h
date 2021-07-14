#ifndef RENDER_H
#define RENDER_H

#include <functional>
#include "Platform.h"

enum class Primitive {
    Points = 0,
    Lines,
    Triangles
};

enum class DrawType {
    Static = 0,
    Dynamic,
    Stream
};

enum class ValueType {
    Byte = 0,
    UByte,
    Short,
    UShort,
    Int,
    UInt,
    HalfFloat,
    Float,
    Double
};

struct Attrib {
    unsigned int Count, Size, Offset;
    ValueType Type;
    bool Normalize;
};

struct DrawData {
    Primitive primitive;
    u32 VAO;
    u32 Count;
};

class DrawObject {
private:
    DrawData data{};

protected:
    virtual void SubmitData(DrawData &drawData) = 0;
    static void ReleaseHandle(const std::function<void()> &deleter);

public:
    DrawObject() = default;
    DrawObject(const DrawObject &obj) { data = {}; }
    DrawObject(DrawObject &&obj) noexcept
    {
        data =  obj.data;
        obj.data = {};
    }

    DrawObject &operator=(const DrawObject &obj) { data = {}; }
    DrawObject &operator=(DrawObject &&obj) noexcept
    {
        data =  obj.data;
        obj.data = {};
    }

    void RenderUpdate() { SubmitData(data); }
    [[nodiscard]] DrawData GetDrawData() const noexcept { return data; }
};

#endif
