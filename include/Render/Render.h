#ifndef RENDER_H
#define RENDER_H

#include <variant>
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

enum class TexType: u8 {
    Texture1D = 0,
    Texture2D,
    TextureCube
};

enum class TexDataType: u8 {
    R = 0,
    RG,
    RGB,
    RGBA,
    R8 = R,
    RG8 = RG,
    RGB8 = RGB,
    RGBA8 = RGBA,
    R16,
    RG16,
    RGB16,
    RGBA16,
    Depth,
    RF,
    RGF,
    RGBF,
    RGBAF,
    R32F = RF,
    RG32F = RGF,
    RGB32F = RGBF,
    RGBA32F = RGBAF,
    R16F,
    RG16F,
    RGB16F,
    RGBA16F
};

enum class TexParams: u8 {
    WrapS = 0,
    WrapT,
    WrapR,
    MinFilter,
    MagFilter,
    MinLOD,
    MaxLOD,
    MaxMipmapLevel
};

enum class WrapType: u8 {
    Repeat = 0,
    MirroredRepeat,
    ClampToEdge,
    ClampToBorder,
    None
};

enum class Filtering: u8 {
    Nearest = 0,
    Linear,
    None
};

struct TexParam {
    TexParams param;
    std::variant<WrapType, Filtering, int> val;
};

struct Attrib {
    u32 Count, Size, Offset;
    ValueType Type;
    bool Normalize;
};

void ReleaseHandle(const std::function<void()> &deleter);

template<typename T>
class RenderObject {
private:
    T data{};

protected:
    virtual void SubmitData(T &drawData) = 0;
    static void ReleaseHandle(const std::function<void()> &deleter)
    {
        ::ReleaseHandle(deleter);
    }

public:
    RenderObject() = default;
    RenderObject(const RenderObject &obj) { data = {}; }
    RenderObject(RenderObject &&obj) noexcept
    {
        data =  obj.data;
        obj.data = {};
    }

    RenderObject &operator=(const RenderObject &obj) { data = {}; return *this; }
    RenderObject &operator=(RenderObject &&obj) noexcept
    {
        data =  obj.data;
        obj.data = {};

        return *this;
    }

    void RenderUpdate() { SubmitData(data); }
    [[nodiscard]] T GetDrawData() const noexcept { return data; }
};

struct DrawData {
    Primitive Primitive;
    u32 VAO;
    u32 Count;
};
using DrawObject = RenderObject<DrawData>;

struct SamplerData {
    u64 Handle;
    TexType TextureType;
    TexDataType TextureDataType;
};
using SamplerObject = RenderObject<SamplerData>;

#endif
