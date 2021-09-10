#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include <string>
#include <filesystem>
#include "Render/Render.h"

namespace fs = std::filesystem;

class ModelImporter {
private:
    std::string currentPath;
    std::string assetName;

    bool isImportMaterials = true;
    bool isImportTextures = true;
    bool isImportAnimations = true;
    bool isImportSkeleton = true;
    bool isImportLights = true;
    bool isImportCameras = true;
    bool isLoadedFinish = true;

    f32 importState = 1.0f;
    std::string_view importStateName;

    void processModel();
    void drawParameters();
    void drawProgressBar();

public:
    void SetCurrentData(const fs::path &path);
    void ModalWindow(const std::string &name);
};

#endif
