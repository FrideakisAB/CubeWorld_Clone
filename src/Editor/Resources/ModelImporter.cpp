#include "Editor/Resources/ModelImporter.h"

#include "imgui.h"
#include "Engine.h"
#include "GameScene.h"
#include "stb_image.h"
#include <assimp/scene.h>
#include "Editor/Editor.h"
#include "Assets/Prefab.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Assets/AssetsManager.h"
#include "Components/Components.h"
#include "Editor/ImGui/ImFileDialog.h"

GameScene ModelImporter::gameScene;

ModelImporter::ModelImporter()
{
    progressHandler = new ModelProgressHandler(importState);
}

void ModelImporter::SetCurrentData(const fs::path &path)
{
    currentPath = path.string();
    assetName = path.stem().string();

    isImportMaterials = true;
    isImportTextures = true;
    isImportAnimations = true;
    isImportSkeleton = true;
    isImportLights = true;
    isImportCameras = true;
}

void ModelImporter::ModalWindow(const std::string &name)
{
    ImVec2 center = ImGui::GetIO().DisplaySize;
    center.x /= 2.0f;
    center.y /= 2.0f;

    ImGui::SetNextWindowPos(center, 0, ImVec2(0.5f, 0.5f));
    if (ImGui::BeginPopupModal(name.c_str(), nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoMove))
    {
        if (isLoadedFinish)
            drawParameters();
        else
            drawProgressBar();

        ImGui::EndPopup();
    }
}

void ModelImporter::processModel()
{
    importState = 0.0f;
    importStateName = "Load raw model";

    static Assimp::Importer importer;
    importer.SetProgressHandler(progressHandler);
    int flags = aiProcess_GenSmoothNormals |
                aiProcess_JoinIdenticalVertices |
                aiProcess_ImproveCacheLocality |
                aiProcess_LimitBoneWeights |
                aiProcess_RemoveRedundantMaterials |
                aiProcess_SplitLargeMeshes |
                aiProcess_Triangulate |
                aiProcess_GenUVCoords |
                aiProcess_TransformUVCoords |
                aiProcess_SortByPType |
                aiProcess_FindDegenerates |
                aiProcess_FindInvalidData;
    const aiScene *scene = importer.ReadFile(currentPath, flags);

    if (scene == nullptr || (scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE) != 0 || scene->mRootNode == nullptr)
    {
        logger->Error(importer.GetErrorString());
        isProcessingFinish = true;
        return;
    }

    std::map<u32, AssetsHandle> materials;
    f32 incValue;
    if (isImportMaterials)
    {
        importState = 0.0f;
        importStateName = "Import materials";

        if (scene->HasMaterials())
        {
            incValue = 1.0f / static_cast<f32>(scene->mNumMaterials);
            for (u32 i = 0; i < scene->mNumMaterials; ++i)
            {
                importState += incValue;
                if (importState > 1.0f)
                    importState = 1.0f;
                materials[i] = importMaterial(assetName + "_Material_Texture_" + std::to_string(i), scene->mMaterials[i]);
            }
        }
        else
            importState = 1.0f;
    }

    importState = 0.0f;
    importStateName = "Process hierarchy";

    GameObject *go = gameScene.Create(assetName);
    go->AddComponent<Transform>();
    incValue = 1.0f / static_cast<f32>(scene->mRootNode->mNumChildren);
    for (u32 i = 0; i < scene->mRootNode->mNumChildren; ++i)
    {
        importState += incValue;
        if (importState > 1.0f)
            importState = 1.0f;
        recAdd(scene, scene->mRootNode->mChildren[i], go, materials);
    }

    if (isImportLights)
    {
        importState = 0.0f;
        importStateName = "Process lights";

        if (scene->HasLights())
        {
            incValue = 1.0f / static_cast<f32>(scene->mNumLights);
            for (u32 i = 0; i < scene->mNumLights; ++i)
            {
                importState += incValue;
                if (importState > 1.0f)
                    importState = 1.0f;
                importLight(scene->mLights[i], go);
            }
        }
        else
            importState = 1.0f;
    }

    if (isImportCameras)
    {
        importState = 0.0f;
        importStateName = "Process cameras";

        if (scene->HasCameras())
        {
            incValue = 1.0f / static_cast<f32>(scene->mNumCameras);
            for (u32 i = 0; i < scene->mNumCameras; ++i)
            {
                importState += incValue;
                if (importState > 1.0f)
                    importState = 1.0f;
                importCamera(scene->mCameras[i], go);
            }
        }
        else
            importState = 1.0f;
    }

    importState = 0.0f;
    importStateName = "Save asset";

    AssetsHandle handle = std::make_shared<Prefab>(*go);
    GameEngine->GetAssetsManager().AddAsset(assetName, handle);
    importState = 0.5f;
    GameEditor->GetAssetsWriter().AddAsset(handle);

    gameScene.Delete(go);

    importState = 1.0f;

    importer.FreeScene();

    isProcessingFinish = true;
}

void ModelImporter::recAdd(const aiScene *sceneModel, aiNode *node, GameObject *go, const std::map<u32, AssetsHandle> &materials)
{
    if (node->mNumMeshes != 0 || node->mNumChildren != 0)
    {
        Position pos;

        aiVector3t<f32> scaling;
        aiVector3t<f32> rotation;
        aiVector3t<f32> position;
        node->mTransformation.Decompose(scaling, rotation, position);

        pos.position = {position.x, position.y, position.z};
        pos.scale = {scaling.x / 100.0f, scaling.y / 100.0f, scaling.z / 100.0f};
        pos.rotate = glm::quat(glm::vec3(rotation.x, rotation.y, rotation.z));

        GameObject *parent;
        if (node->mNumMeshes != 1)
        {
            auto *childGameObject = gameScene.Create(node->mName.C_Str());
            parent = childGameObject;
            go->AddChild(childGameObject);

            for (u32 i = 0; i < node->mNumMeshes; ++i)
            {
                std::string meshName = sceneModel->mMeshes[node->mMeshes[i]]->mName.C_Str();
                if (meshName.empty())
                    meshName = std::string("MultipleMesh_") + std::to_string(i);

                auto *subChildGO = GameEngine->GetGameScene().Create(meshName);

                if (auto It = materials.find(sceneModel->mMeshes[node->mMeshes[i]]->mMaterialIndex); It != materials.end())
                    childGameObject->AddComponent<MaterialComponent>()->SetMaterial(It->second);
                else
                {
                    auto materialHandle = std::make_shared<Material>();
                    auto *material = static_cast<Material*>(materialHandle.get());
                    material->Shader = "BlinnPhong";
                    material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 1.0f, 1.0f)};
                    material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
                    childGameObject->AddComponent<MaterialComponent>()->SetMaterial(materialHandle);
                }

                subChildGO->AddComponent<Transform>()->SetLocalPos(pos);
                auto *meshComp = subChildGO->AddComponent<MeshComponent>();
                meshComp->SetMesh(convertMesh(sceneModel->mMeshes[node->mMeshes[i]]));

                meshName = assetName + "_" + std::string(node->mName.C_Str()) + "_" + std::to_string(i);
                GameEngine->GetAssetsManager().AddAsset(meshName, meshComp->GetAsset());
                GameEditor->GetAssetsWriter().AddAsset(meshComp->GetAsset());

                childGameObject->AddChild(subChildGO);
            }
        }
        else if (node->mNumMeshes != 0)
        {
            std::string meshName = node->mName.C_Str();
            auto *childGameObject = gameScene.Create(meshName);
            parent = childGameObject;

            if (auto It = materials.find(sceneModel->mMeshes[node->mMeshes[0]]->mMaterialIndex); It != materials.end())
                childGameObject->AddComponent<MaterialComponent>()->SetMaterial(It->second);
            else
            {
                auto materialHandle = std::make_shared<Material>();
                auto *material = static_cast<Material*>(materialHandle.get());
                material->Shader = "BlinnPhong";
                material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 1.0f, 1.0f)};
                material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
                childGameObject->AddComponent<MaterialComponent>()->SetMaterial(materialHandle);
            }

            childGameObject->AddComponent<Transform>()->SetLocalPos(pos);
            auto *meshComp = childGameObject->AddComponent<MeshComponent>();
            meshComp->SetMesh(convertMesh(sceneModel->mMeshes[node->mMeshes[0]]));

            meshName = assetName + "_" + std::string(node->mName.C_Str()) + "_" + std::string(sceneModel->mMeshes[node->mMeshes[0]]->mName.C_Str());
            GameEngine->GetAssetsManager().AddAsset(meshName, meshComp->GetAsset());
            GameEditor->GetAssetsWriter().AddAsset(meshComp->GetAsset());

            go->AddChild(childGameObject);
        }
        else
        {
            auto *childGameObject = gameScene.Create(node->mName.C_Str());
            parent = childGameObject;

            childGameObject->AddComponent<Transform>()->SetLocalPos(pos);

            go->AddChild(childGameObject);
        }

        for (u32 i = 0; i < node->mNumChildren; ++i)
            recAdd(sceneModel, node->mChildren[i], parent, materials);
    }
}

void ModelImporter::importLight(aiLight *light, GameObject *go)
{
    if (light->mType == aiLightSource_DIRECTIONAL ||
        light->mType == aiLightSource_SPOT ||
        light->mType == aiLightSource_POINT)
    {
        auto *lightGameObject = gameScene.Create(light->mName.C_Str());
        go->AddChild(lightGameObject);

        Position pos;
        pos.position = {light->mPosition.x, light->mPosition.y, light->mPosition.z};
        if (light->mType == aiLightSource_DIRECTIONAL || light->mType == aiLightSource_SPOT)
        {
            glm::vec3 direction = {light->mDirection.x, light->mDirection.y, light->mDirection.z};
            glm::mat4 lightLookAt = glm::lookAt(glm::vec3(0.0f, -1.0f, 0.0f), direction, glm::vec3(0.0f, 1.0f, 0.0f));
            pos.rotate = glm::quat_cast(lightLookAt);
        }

        lightGameObject->AddComponent<Transform>()->SetLocalPos(pos);
        auto *lightSource = lightGameObject->AddComponent<LightSource>();
        lightSource->Color = {light->mColorDiffuse.r / 1000.0f, light->mColorDiffuse.g / 1000.0f, light->mColorDiffuse.b / 1000.0f};

        switch (light->mType)
        {
        case aiLightSource_DIRECTIONAL:
            lightSource->Type = LightType::Directional;
            break;

        case aiLightSource_SPOT:
            lightSource->Type = LightType::Spot;
            lightSource->CutterOff = light->mAngleOuterCone;
            lightSource->Radius = 100.0f;
            break;

        case aiLightSource_POINT:
            lightSource->Type = LightType::Point;
            lightSource->Radius = 100.0f;
            break;
        }
    }
}

void ModelImporter::importCamera(aiCamera *camera, GameObject *go)
{
    auto *cameraGameObject = gameScene.Create(camera->mName.C_Str());
    go->AddChild(cameraGameObject);

    Position pos;
    pos.position = {camera->mPosition.x, camera->mPosition.y, camera->mPosition.z};

    glm::vec3 cameraLookAt = {camera->mLookAt.x, camera->mLookAt.y, camera->mLookAt.z};
    pos.rotate = glm::quat_cast(glm::lookAt(glm::vec3(0.0f, 0.0f, 1.0f), cameraLookAt, glm::vec3(0.0f, 1.0f, 0.0f)));

    cameraGameObject->AddComponent<Transform>()->SetLocalPos(pos);
    auto *resCamera = cameraGameObject->AddComponent<Camera>();
    resCamera->NearClip = camera->mClipPlaneNear;
    resCamera->FarClip = camera->mClipPlaneFar;
    resCamera->Fov = glm::degrees(camera->mHorizontalFOV);
}

#define AI_MATKEY_SPECULAR "$mat.specular",0,0
AssetsHandle ModelImporter::importMaterial(const std::string &assetName, aiMaterial *materialAssimp)
{
    aiColor3D color;
    materialAssimp->Get(AI_MATKEY_COLOR_DIFFUSE, color);

    aiColor3D bloom;
    materialAssimp->Get(AI_MATKEY_COLOR_EMISSIVE, bloom);

    f32 specular;
    materialAssimp->Get(AI_MATKEY_SPECULAR, specular);
    specular = glm::clamp(specular, 0.0f, 1.0f);

    aiString textureDiffuseAssimp;
    aiTextureMapMode wrapDiffuseU, wrapDiffuseV;
    materialAssimp->Get(AI_MATKEY_TEXTURE_DIFFUSE(0), textureDiffuseAssimp);
    materialAssimp->Get(AI_MATKEY_MAPPINGMODE_U_DIFFUSE(0), wrapDiffuseU);
    materialAssimp->Get(AI_MATKEY_MAPPINGMODE_V_DIFFUSE(0), wrapDiffuseV);

    AssetsHandle textureDiffuse = importTexture(assetName, textureDiffuseAssimp, wrapDiffuseU, wrapDiffuseV);

    if (!bloom.IsBlack())
    {
        AssetsHandle materialHandle = std::make_shared<Material>();
        auto *material = static_cast<Material*>(materialHandle.get());
        material->Shader = "Bloom";
        material->Uniforms["color_light"] = {Utils::ShaderValue::Vector3, glm::vec3(bloom.r, bloom.g, bloom.b)};

        return materialHandle;
    }
    else if (textureDiffuse)
    {
        AssetsHandle materialHandle = std::make_shared<Material>();
        auto *material = static_cast<Material*>(materialHandle.get());
        material->Shader = "BlinnPhongTex";
        material->Samplers["texture_diffuse"] = textureDiffuse;
        material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, specular};

        return materialHandle;
    }
    else
    {
        AssetsHandle materialHandle = std::make_shared<Material>();
        auto *material = static_cast<Material*>(materialHandle.get());
        material->Shader = "BlinnPhong";
        material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(color.r, color.g, color.b)};
        material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, specular};

        return materialHandle;
    }
}

AssetsHandle ModelImporter::importTexture(const std::string &assetName, const aiString &path, aiTextureMapMode mapModeU, aiTextureMapMode mapModeV)
{
    if (fs::exists(fs::path(path.C_Str())))
    {
        int x, y, components;
        u8 *src = stbi_load(path.C_Str(), &x, &y, &components, 0);

        AssetsHandle texture = std::make_shared<Texture>();
        auto *texturePtr = static_cast<Texture*>(texture.get());
        texturePtr->SetType(TexType::Texture2D);
        texturePtr->SetFiltering(Filtering::Linear);
        texturePtr->SetWH({x, y});

        switch (mapModeU)
        {
        case aiTextureMapMode_Wrap:
            texturePtr->SetWrapS(WrapType::Repeat);
            break;

        case aiTextureMapMode_Clamp:
            texturePtr->SetWrapS(WrapType::ClampToEdge);
            break;

        case aiTextureMapMode_Mirror:
            texturePtr->SetWrapS(WrapType::MirroredRepeat);
            break;
        }

        switch (mapModeV)
        {
        case aiTextureMapMode_Wrap:
            texturePtr->SetWrapT(WrapType::Repeat);
            break;

        case aiTextureMapMode_Clamp:
            texturePtr->SetWrapT(WrapType::ClampToEdge);
            break;

        case aiTextureMapMode_Mirror:
            texturePtr->SetWrapT(WrapType::MirroredRepeat);
            break;
        }

        texturePtr->SetMipmap(true);

        switch (components)
        {
        case 1:
            texturePtr->SetTypeData(TexDataType::R);
            break;
        case 2:
            texturePtr->SetTypeData(TexDataType::RG);
            break;
        case 3:
            texturePtr->SetTypeData(TexDataType::RGB);
            break;
        case 4:
            texturePtr->SetTypeData(TexDataType::RGBA);
            break;
        }

        u8 *dataCopy = new u8[x * y * components];
        std::copy(src, src + x * y * components, dataCopy);
        stbi_image_free(src);

        texturePtr->SetSrc(dataCopy);
        texturePtr->Apply();
        texturePtr->IsStatic = true;

        GameEngine->GetAssetsManager().AddAsset(assetName, texture);
        GameEditor->GetAssetsWriter().AddAsset(texture);

        return texture;
    }

    return {};
}

Mesh *ModelImporter::convertMesh(aiMesh *mesh)
{
    auto *vertices = new Mesh::Vertex[mesh->mNumVertices];

    u32 indicesCount = 0;
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
        indicesCount += mesh->mFaces[i].mNumIndices;

    auto *indices = new u32[indicesCount];

    for (u32 i = 0; i < mesh->mNumVertices; ++i)
    {
        vertices[i].Position = {mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z};
        vertices[i].Normal = {mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z};

        if (mesh->mTextureCoords[0] != nullptr)
            vertices[i].TexCoords = {mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y};
        else
            vertices[i].TexCoords = {0.0f, 0.0f};
    }

    u32 indicesPosition = 0;
    for (u32 i = 0; i < mesh->mNumFaces; ++i)
    {
        aiFace &face = mesh->mFaces[i];
        for (u32 j = 0; j < face.mNumIndices; ++j)
            indices[indicesPosition++] = face.mIndices[j];
    }

    Mesh *newMesh = new Mesh(vertices, mesh->mNumVertices, indices, indicesCount);
    return newMesh;
}

void ModelImporter::drawParameters()
{
    ImGui::Text("Path: %s", currentPath.c_str());
    ImGui::Separator();

    ImGui::Checkbox("Import material", &isImportMaterials);
    ImGui::Checkbox("Import textures", &isImportTextures);
    ImGui::Checkbox("Import animations", &isImportAnimations);
    ImGui::Checkbox("Import skeleton", &isImportSkeleton);
    ImGui::Checkbox("Import lights", &isImportLights);
    ImGui::Checkbox("Import cameras", &isImportCameras);

    std::string label = "Asset name";
    bool isUniqName = GameEngine->GetAssetsManager().GetAsset(assetName) == nullptr;
    if (!isUniqName)
        label += "(Not uniq)";
    else
        label += "(Uniq)";

    char buffer[256] = {0};
    if (assetName.size() < 256)
        std::copy(assetName.begin(), assetName.end(), buffer);
    ImGui::InputText(label.c_str(), buffer, IM_ARRAYSIZE(buffer));
    assetName = buffer;
    ImGui::Separator();

    if (ImGui::Button("Apply", ImVec2(150, 0)) && isUniqName)
    {
        if (loadingFuture.valid())
            loadingFuture.get();
        loadingFuture = std::async(std::launch::async, &ModelImporter::processModel, this);
        isLoadedFinish = false;
        isProcessingFinish = false;
    }

    ImGui::SameLine();
    ImGui::SetItemDefaultFocus();
    if (ImGui::Button("Cancel", ImVec2(150, 0)))
        ImGui::CloseCurrentPopup();
}

void ModelImporter::drawProgressBar()
{
    ImGui::ProgressBar(importState);
    ImGui::Text("%s", importStateName.data());
    if (isProcessingFinish)
    {
        isLoadedFinish = true;
        ImGui::CloseCurrentPopup();
    }
}
