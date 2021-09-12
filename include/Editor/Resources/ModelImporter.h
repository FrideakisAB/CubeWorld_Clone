#ifndef MODELIMPORTER_H
#define MODELIMPORTER_H

#include <string>
#include <future>
#include <filesystem>
#include "GameScene.h"
#include <assimp/mesh.h>
#include "Render/Mesh.h"
#include "Render/Render.h"
#include <assimp/scene.h>
#include "Assets/AssetsManager.h"
#include <assimp/ProgressHandler.hpp>

namespace fs = std::filesystem;

class GameObject;

class ModelImporter {
    class ModelProgressHandler final : public Assimp::ProgressHandler {
    private:
        f32 &progress;

    public:
        explicit ModelProgressHandler(f32 &progress) : progress(progress) {}

        bool Update(f32 state) final
        {
            progress = state;
            return true;
        }
    };

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
    bool isProcessingFinish = false;

    f32 importState = 1.0f;
    std::string_view importStateName;
    ModelProgressHandler *progressHandler;
    std::future<void> loadingFuture;
    static GameScene gameScene;

    void processModel();
    void drawParameters();
    void drawProgressBar();
    void recAdd(const aiScene *sceneModel, aiNode *node, GameObject *go, const std::map<u32, AssetsHandle> &materials);

    static Mesh *convertMesh(aiMesh *mesh);
    static void importLight(aiLight *light, GameObject *go);
    static void importCamera(aiCamera *camera, GameObject *go);
    static AssetsHandle importMaterial(aiMaterial *materialAssimp);

public:
    ModelImporter();

    void SetCurrentData(const fs::path &path);
    void ModalWindow(const std::string &name);
};

#endif
