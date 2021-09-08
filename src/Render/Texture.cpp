#include "Render/Texture.h"

#include "Render/GLUtils.h"
#include "stb_image_write.h"

Texture::Texture(const Texture &texture)
    : IAsset(true), SamplerObject(texture)
{
    whd = texture.whd;
    wrapS = texture.wrapS;
    wrapT = texture.wrapT;
    wrapR = texture.wrapR;
    filtering = texture.filtering;
    tt = texture.tt;
    type = texture.type;
    mipmaps = texture.mipmaps;
}

Texture::Texture(Texture &&texture) noexcept
    : IAsset(true), SamplerObject(std::move(texture))
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
    if (texture != 0)
    {
        for (auto &i : src)
            delete[] i;

        auto deleter = [=]() {
            glDeleteTextures(1, &texture);
        };

        ReleaseHandle(deleter);
    }
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
    if (applyRequire)
    {
        if (texture != 0)
            glDeleteTextures(1, &texture);

        glGenTextures(1, &texture);

        switch (tt)
        {
        case TexType::Texture1D:
            glBindTexture(GL_TEXTURE_1D, texture);
            glTexImage1D(GL_TEXTURE_1D, 0, Utils::GetTextDataTypeGL(type), whd.x, 0, Utils::GetTextDataTypeGL(type), GL_UNSIGNED_BYTE, src[0]);

            glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, Utils::GetWrapTypeGL(wrapS));

            if (filtering != Filtering::None)
            {
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, Utils::GetFilteringGL(filtering));
                glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, Utils::GetFilteringGL(filtering));
            }

            if (mipmaps)
                glGenerateMipmap(GL_TEXTURE_1D);
            glBindTexture(GL_TEXTURE_1D, 0);
            break;

        case TexType::Texture2D:
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(GL_TEXTURE_2D, 0, Utils::GetTextDataTypeGL(type), whd.x, whd.y, 0, Utils::GetTextDataTypeGL(type), GL_UNSIGNED_BYTE, src[0]);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, Utils::GetWrapTypeGL(wrapS));
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, Utils::GetWrapTypeGL(wrapT));

            if (filtering != Filtering::None)
            {
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, Utils::GetFilteringGL(filtering));
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, Utils::GetFilteringGL(filtering));
            }

            if (mipmaps)
                glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
            break;

        case TexType::TextureCube:
            glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
            for(GLuint i = 0; i < 6; ++i)
                glTexImage2D(
                        GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, Utils::GetTextDataTypeGL(type), whd.x, whd.y,
                        0,Utils::GetTextDataTypeGL(type), GL_UNSIGNED_BYTE, src[i]
                );

            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, Utils::GetWrapTypeGL(wrapS));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, Utils::GetWrapTypeGL(wrapT));
            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, Utils::GetWrapTypeGL(wrapR));

            if (filtering != Filtering::None)
            {
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, Utils::GetFilteringGL(filtering));
                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, Utils::GetFilteringGL(filtering));
            }

            if (mipmaps)
                glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
            glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
            break;
        }

        samplerData.Handle = texture;
        samplerData.TextureDataType = type;
        samplerData.TextureType = tt;

        applyRequire = false;
    }
    else
    {
        samplerData.Handle = texture;
        samplerData.TextureDataType = type;
        samplerData.TextureType = tt;
    }
}

IAsset *Texture::Clone() const
{
    return new Texture(std::cref(*this));
}

json Texture::SerializeObj()
{
    json data;

    data["nameType"] = boost::typeindex::type_id<Texture>().pretty_name();

    data["whd"] = {whd.x, whd.y, whd.z};

    data["wrapS"] = static_cast<u8>(wrapS);
    data["wrapT"] = static_cast<u8>(wrapT);
    data["wrapR"] = static_cast<u8>(wrapR);

    data["filtering"] = static_cast<u8>(filtering);
    data["tt"] = static_cast<u8>(tt);
    data["type"] = static_cast<u8>(type);

    data["mipmaps"] = mipmaps;

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

    Apply();
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

void Texture::SerializeBin(std::ofstream &file)
{
    if (file.is_open())
    {
        u8 pixelSize;
        switch(type)
        {
        case TexDataType::R: [[fallthrough]];
        case TexDataType::Depth:
            pixelSize = 1;
            break;

        case TexDataType::RG: [[fallthrough]];
        case TexDataType::R16: [[fallthrough]];
        case TexDataType::R16F:
            pixelSize = 2;
            break;

        case TexDataType::RGB:
            pixelSize = 3;
            break;

        case TexDataType::RGBA: [[fallthrough]];
        case TexDataType::RG16: [[fallthrough]];
        case TexDataType::RF: [[fallthrough]];
        case TexDataType::RG16F:
            pixelSize = 4;
            break;

        case TexDataType::RGF: [[fallthrough]];
        case TexDataType::RGBA16F: [[fallthrough]];
        case TexDataType::RGBA16:
            pixelSize = 8;
            break;

        case TexDataType::RGBF:
            pixelSize = 12;
            break;

        case TexDataType::RGBAF:
            pixelSize = 16;
            break;
        case TexDataType::RGB16: [[fallthrough]];
        case TexDataType::RGB16F:
            pixelSize = 6;
            break;
        }

        for (auto &i : src)
            if(i != nullptr)
                file.write(reinterpret_cast<char*>(i), whd.x * whd.y * pixelSize);
    }
}

void Texture::UnSerializeBin(std::ifstream &file)
{
    if (file.is_open())
    {
        u8 pixelSize;
        switch (type)
        {
        case TexDataType::R: [[fallthrough]];
        case TexDataType::Depth:
            pixelSize = 1;
            break;

        case TexDataType::RG: [[fallthrough]];
        case TexDataType::R16: [[fallthrough]];
        case TexDataType::R16F:
            pixelSize = 2;
            break;

        case TexDataType::RGB:
            pixelSize = 3;
            break;

        case TexDataType::RGBA: [[fallthrough]];
        case TexDataType::RG16: [[fallthrough]];
        case TexDataType::RF: [[fallthrough]];
        case TexDataType::RG16F:
            pixelSize = 4;
            break;

        case TexDataType::RGF: [[fallthrough]];
        case TexDataType::RGBA16F: [[fallthrough]];
        case TexDataType::RGBA16:
            pixelSize = 8;
            break;

        case TexDataType::RGBF:
            pixelSize = 12;
            break;

        case TexDataType::RGBAF:
            pixelSize = 16;
            break;
        case TexDataType::RGB16: [[fallthrough]];
        case TexDataType::RGB16F:
            pixelSize = 6;
            break;
        }

        if (tt == TexType::TextureCube)
            for (auto &i : src)
            {
                i = new u8[whd.x * whd.y * pixelSize];
                file.read(reinterpret_cast<char*>(i), whd.x * whd.y * pixelSize);
            }
        else
        {
            src[0] = new u8[whd.x * whd.y * pixelSize];
            file.read(reinterpret_cast<char*>(src[0]), whd.x * whd.y * pixelSize);
        }
    }
}

void Texture::SetType(TexType type)
{
    tt = type;
}
