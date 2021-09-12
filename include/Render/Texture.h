#ifndef TEXTURE_H
#define TEXTURE_H

#include "Render/Render.h"
#include "Assets/IAsset.h"
#include "boost/type_index.hpp"

class Texture final : public SamplerObject, public IAsset {
protected:
    void SubmitData(SamplerData &samplerData) final;

private:
    u8 *src[6] = {};
    u32 textureId = 0;
    glm::uvec3 whd = glm::uvec3(1);
    WrapType wrapS = WrapType::Repeat;
    WrapType wrapT = WrapType::Repeat;
    WrapType wrapR = WrapType::Repeat;
    Filtering filtering = Filtering::None;
    TexType tt = TexType::Texture2D;
    TexDataType type = TexDataType::RGB;
    bool mipmaps = false;
    bool applyRequire = true;

public:
    Texture() : IAsset(true) {}
    Texture(const Texture &texture);
    Texture(Texture &&texture) noexcept;
    ~Texture() final;

    Texture &operator=(const Texture &texture);
    Texture &operator=(Texture &&texture) noexcept;

    [[nodiscard]] inline u32 GetW() const noexcept { return whd.x; }
    void SetW(u32 whd);

    [[nodiscard]] inline glm::uvec2 GetWH() const noexcept { return {whd.x, whd.y}; }
    void SetWH(glm::uvec2 whd);

    [[nodiscard]] inline glm::uvec3 GetWHD() const noexcept { return whd; }
    void SetWHD(glm::uvec3 whd);

    [[nodiscard]] inline u8 *GetSrc() const noexcept { return src[0]; }
    void SetSrc(u8 *src);

    [[nodiscard]] inline u8 *GetSrcCubeTexture(u8 i) { return src[i]; }
    void SetSrcCubeTexture(u8 *src, u8 i);

    [[nodiscard]] inline Filtering GetFiltering() const noexcept { return filtering; }
    void SetFiltering(Filtering filt);

    [[nodiscard]] inline WrapType GetWrapS() const noexcept { return wrapS; }
    void SetWrapS(WrapType wrap);

    [[nodiscard]] inline WrapType GetWrapT() const noexcept { return wrapT; }
    void SetWrapT(WrapType wrap);

    [[nodiscard]] inline WrapType GetWrapR() const noexcept { return wrapR; }
    void SetWrapR(WrapType wrap);

    [[nodiscard]] inline TexDataType GetTypeData() const noexcept { return type; }
    void SetTypeData(TexDataType type);

    [[nodiscard]] inline TexType GetType() const noexcept { return tt; }
    void SetType(TexType type);

    [[nodiscard]] inline bool GetMipmap() const noexcept { return mipmaps; }
    void SetMipmap(bool mipmaps);

    void Apply() noexcept;
    [[nodiscard]] bool IsApply() const noexcept { return !applyRequire; }

    [[nodiscard]] size_t GetTypeID() const noexcept final
    {
        return boost::typeindex::type_id<Texture>().hash_code();
    }

    [[nodiscard]] IAsset *Clone() const final;

    [[nodiscard]] json SerializeObj() const final;
    void UnSerializeObj(const json& j) final;

    void SerializeBin(std::ofstream &file) final;
    void UnSerializeBin(std::ifstream &file) final;

    bool IsStatic = false;
};

#endif
