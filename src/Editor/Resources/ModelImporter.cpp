#include "Editor/Resources/ModelImporter.h"

#include "imgui.h"
#include "Engine.h"
#include "GameScene.h"
#include <assimp/scene.h>
#include "Editor/Editor.h"
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include "Assets/AssetsManager.h"
#include "Components/Components.h"
#include "Editor/ImGui/ImFileDialog.h"

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

    Assimp::Importer importer;
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

    importState = 0.0f;
    importStateName = "Process hierarchy";

    GameObject *go = GameEngine->GetGameScene().Create(assetName);
    go->AddComponent<Transform>();
    f32 incValue = 1.0f / static_cast<f32>(scene->mRootNode->mNumChildren);
    for (u32 i = 0; i < scene->mRootNode->mNumChildren; ++i)
    {
        importState += incValue;
        if (importState > 1.0f)
            importState = 1.0f;
        recAdd(scene, scene->mRootNode->mChildren[i], go);
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

    isProcessingFinish = true;
}

void ModelImporter::recAdd(const aiScene *sceneModel, aiNode *node, GameObject *go)
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

        GameObject *parent = nullptr;

        if (node->mNumMeshes != 1)
        {
            auto *childGameObject = GameEngine->GetGameScene().Create(node->mName.C_Str());
            parent = childGameObject;
            go->AddChild(childGameObject);

            for (u32 i = 0; i < node->mNumMeshes; ++i)
            {
                std::string meshName = sceneModel->mMeshes[node->mMeshes[i]]->mName.C_Str();
                if (meshName.empty())
                    meshName = std::string("MultipleMesh_") + std::to_string(i);

                auto *subChildGO = GameEngine->GetGameScene().Create(meshName);

                auto materialHandle = std::make_shared<Material>();
                auto *material = static_cast<Material*>(materialHandle.get());
                material->Shader = "LightAccumulation";
                Utils::ShaderParamValue uniformPlane;
                uniformPlane.value = glm::vec3(1.0f, 0.0f, 0.0f);
                uniformPlane.valueType = Utils::ShaderValue::Vector3;
                material->Uniforms["color_diffuse"] = uniformPlane;
                uniformPlane.value = 0.1f;
                uniformPlane.valueType = Utils::ShaderValue::Float;
                material->Uniforms["main_specular"] = uniformPlane;
                childGameObject->AddComponent<MaterialComponent>()->SetMaterial(materialHandle);
                subChildGO->AddComponent<Transform>()->SetLocalPos(pos);
                auto *meshComp = subChildGO->AddComponent<MeshComponent>();
                meshComp->SetMesh(convertMesh(sceneModel->mMeshes[node->mMeshes[i]]));

                childGameObject->AddChild(subChildGO);
            }
        }
        else if (node->mNumMeshes != 0)
        {
            auto *childGameObject = GameEngine->GetGameScene().Create(node->mName.C_Str());
            parent = childGameObject;

            auto materialHandle = std::make_shared<Material>();
            auto *material = static_cast<Material*>(materialHandle.get());
            material->Shader = "LightAccumulation";
            Utils::ShaderParamValue uniformPlane;
            uniformPlane.value = glm::vec3(1.0f, 0.0f, 0.0f);
            uniformPlane.valueType = Utils::ShaderValue::Vector3;
            material->Uniforms["color_diffuse"] = uniformPlane;
            uniformPlane.value = 0.1f;
            uniformPlane.valueType = Utils::ShaderValue::Float;
            material->Uniforms["main_specular"] = uniformPlane;
            childGameObject->AddComponent<MaterialComponent>()->SetMaterial(materialHandle);
            childGameObject->AddComponent<Transform>()->SetLocalPos(pos);
            auto *meshComp = childGameObject->AddComponent<MeshComponent>();
            meshComp->SetMesh(convertMesh(sceneModel->mMeshes[node->mMeshes[0]]));

            go->AddChild(childGameObject);
        }
        else
        {
            auto *childGameObject = GameEngine->GetGameScene().Create(node->mName.C_Str());
            parent = childGameObject;

            childGameObject->AddComponent<Transform>()->SetLocalPos(pos);

            go->AddChild(childGameObject);
        }

        for (u32 i = 0; i < node->mNumChildren; ++i)
            recAdd(sceneModel, node->mChildren[i], parent);
    }
}

void ModelImporter::importLight(aiLight *light, GameObject *go)
{
    if (light->mType == aiLightSource_DIRECTIONAL ||
        light->mType == aiLightSource_SPOT ||
        light->mType == aiLightSource_POINT)
    {
        auto *lightGameObject = GameEngine->GetGameScene().Create(light->mName.C_Str());
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
        lightSource->Color = {light->mColorDiffuse.r / 100.0f, light->mColorDiffuse.g / 100.0f, light->mColorDiffuse.b / 100.0f};

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
    auto* cameraGameObject = GameEngine->GetGameScene().Create(camera->mName.C_Str());
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
