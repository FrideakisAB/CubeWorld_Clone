#include "Render/Texture.h"

Texture::Texture(const Texture &texture)
    : SamplerObject(texture)
{
    whd = texture.whd;
    wrapS = texture.wrapS;
    wrapT = texture.wrapT;
    wrapR = texture.wrapR;
    filtering = texture.filtering;
    tt = texture.tt;
    type = texture.type;
    mipmaps = texture.mipmaps;

    //TODO: src copy problem
}

Texture::Texture(Texture &&texture) noexcept
    : SamplerObject(std::move(texture))
{
    whd = texture.whd;
    wrapS = texture.wrapS;
    wrapT = texture.wrapT;
    wrapR = texture.wrapR;
    filtering = texture.filtering;
    tt = texture.tt;
    type = texture.type;
    mipmaps = texture.mipmaps;
    applyRequire = texture.applyRequire;

    for (u8 i = 0; i < 6; ++i)
        std::swap(src[i], texture.src[i]);
}

Texture::~Texture()
{
    auto deleter = [=](){
        //TODO: on graphics api integrate
    };
    ReleaseHandle(deleter);
}

void Texture::SetW(u32 whd)
{
    this->whd = glm::uvec3(whd, 1, 1);
}

void Texture::SetWH(glm::uvec2 whd)
{
    this->whd = glm::uvec3(whd, 1);
}

void Texture::SetWHD(glm::uvec3 whd)
{
    this->whd = whd;
}

void Texture::SetSrc(u8 *src)
{
    this->src[0] = src;
}

void Texture::SetSrcCubeTexture(u8 *src, u8 i)
{
    this->src[i] = src;
}

void Texture::SetFiltering(Filtering filt)
{
    filtering = filt;
}

void Texture::SetWrapS(WrapType wrap)
{
    wrapS = wrap;
}

void Texture::SetWrapT(WrapType wrap)
{
    wrapT = wrap;
}

void Texture::SetWrapR(WrapType wrap)
{
    wrapR = wrap;
}

void Texture::SetTypeData(TexDataType type)
{
    this->type = type;
}

void Texture::SetMipmap(bool mipmaps)
{
    this->mipmaps = mipmaps;
}

void Texture::Apply() noexcept
{
    applyRequire = true;
}

void Texture::SubmitData(SamplerData &samplerData)
{

}

IAsset *Texture::Clone() const
{
    return new Texture(std::cref(*this));
}

json Texture::SerializeObj()
{
    json data;

    data["whd"] = {whd.x, whd.y, whd.z};

    data["wrapS"] = static_cast<u8>(wrapS);
    data["wrapT"] = static_cast<u8>(wrapT);
    data["wrapR"] = static_cast<u8>(wrapR);

    data["filtering"] = static_cast<u8>(filtering);
    data["tt"] = static_cast<u8>(tt);
    data["type"] = static_cast<u8>(type);

    data["mipmaps"] = mipmaps;

    //TODO: save src, binary resource problem

    return data;
}

void Texture::UnSerializeObj(const json &j)
{
    whd = glm::uvec3(j["whd"][0], j["whd"][1], j["whd"][2]);

    wrapS = static_cast<WrapType>(j["wrapS"].get<u8>());
    wrapT = static_cast<WrapType>(j["wrapT"].get<u8>());
    wrapR = static_cast<WrapType>(j["wrapR"].get<u8>());

    filtering = static_cast<Filtering>(j["filtering"].get<u8>());
    tt = static_cast<TexType>(j["tt"].get<u8>());
    type = static_cast<TexDataType>(j["type"].get<u8>());

    mipmaps = j["mipmaps"];

    //TODO: load src, binary resource problem

    applyRequire = true;
}

Texture &Texture::operator=(const Texture &texture)
{
    whd = texture.whd;
    wrapS = texture.wrapS;
    wrapT = texture.wrapT;
    wrapR = texture.wrapR;
    filtering = texture.filtering;
    tt = texture.tt;
    type = texture.type;
    mipmaps = texture.mipmaps;

    //TODO: src copy problem

    return *this;
}

Texture &Texture::operator=(Texture &&texture) noexcept
{
    whd = texture.whd;
    wrapS = texture.wrapS;
    wrapT = texture.wrapT;
    wrapR = texture.wrapR;
    filtering = texture.filtering;
    tt = texture.tt;
    type = texture.type;
    mipmaps = texture.mipmaps;
    applyRequire = texture.applyRequire;

    for (u8 i = 0; i < 6; ++i)
        std::swap(src[i], texture.src[i]);

    return *this;
}
