#ifndef TEXTUREIMPORTER_H
#define TEXTUREIMPORTER_H

#include <string>
#include <filesystem>
#include "Render/Render.h"

namespace fs = std::filesystem;

class TextureImporter {
private:
    std::string currentPath;
    std::string paths[6]{};
    std::string assetName;
    int width, height, components;

    WrapType wrapS = WrapType::Repeat;
    WrapType wrapT = WrapType::Repeat;
    WrapType wrapR = WrapType::Repeat;
    Filtering filtering = Filtering::Linear;
    bool mipmaps = true;

    u32 currentPos = 0;

    bool cube = false;

public:
    void SetCurrentData(const fs::path &path, int width, int height, int comp);
    void ModalWindow(const std::string &name);
};

#endif
