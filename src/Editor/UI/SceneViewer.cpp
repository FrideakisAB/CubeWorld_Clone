#include "Editor/UI/SceneViewer.h"

#include "Engine.h"
#include "GameScene.h"
#include "Editor/Editor.h"
#include "Assets/Prefab.h"
#include "Utils/Primitives.h"
#include "Components/Components.h"
#include "Editor/Commands/SceneViewCommands.h"

void SceneViewer::Draw()
{
    if (ImGui::BeginDock("Scene viewer", &Active))
    {
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_GO"))
            {
                GameEditor->CommandList.AddCommand<RemoveParent>(*((ECS::EntityId*)payload->Data));
                GameEditor->CommandList.Redo();
                ImGui::EndDragDropTarget();
            }
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
            {
                std::string_view str = *static_cast<std::string_view*>(payload->Data);
                if (GameEngine->GetAssetsManager().GetAsset<Prefab>(str.data()) != nullptr)
                {
                    GameEditor->CommandList.AddCommand<InstantiatePrefab>(std::string(str));
                    GameEditor->CommandList.Redo();
                }
                ImGui::EndDragDropTarget();
            }
        }
        if (ImGui::BeginPopupContextItem("SceneMenu"))
        {
            if (ImGui::BeginMenu("Create..."))
            {
                menu.targetObjectId = ECS::INVALID_ENTITY_ID;
                menu.Draw();
                ImGui::EndMenu();
            }
            ImGui::EndPopup();
        }

        for (auto &&[eid, go] : GameEngine->GetGameScene().gameObjects)
            if (go->GetParentID() == ECS::INVALID_ENTITY_ID)
                recursiveTreeDraw(go->Name, go);

        if (ImVec2 sz = ImGui::GetContentRegionAvail(); sz.x > 0.0f && sz.y > 0.0f)
        {
            if (ImGui::InvisibleButton("SVBottomButton", sz))
                GameEditor->Selected = ECS::INVALID_ENTITY_ID;

            if (ImGui::BeginPopupContextItem("SceneMenu##2"))
            {
                if (ImGui::BeginMenu("Create..."))
                {
                    menu.targetObjectId = ECS::INVALID_ENTITY_ID;
                    menu.Draw();
                    ImGui::EndMenu();
                }
                ImGui::EndPopup();
            }
            if (ImGui::BeginDragDropTarget())
            {
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_GO"))
                {
                    GameEditor->CommandList.AddCommand<RemoveParent>(*((ECS::EntityId*)payload->Data));
                    GameEditor->CommandList.Redo();
                    ImGui::EndDragDropTarget();
                }
                if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
                {
                    std::string_view str = *static_cast<std::string_view*>(payload->Data);
                    if (GameEngine->GetAssetsManager().GetAsset<Prefab>(str.data()) != nullptr)
                    {
                        GameEditor->CommandList.AddCommand<InstantiatePrefab>(std::string(str));
                        GameEditor->CommandList.Redo();
                    }
                    ImGui::EndDragDropTarget();
                }
            }
        }
    }
    ImGui::EndDock();

    if (deletedObjectId != ECS::INVALID_ENTITY_ID)
    {
        if (deletedObjectId == GameEditor->Selected)
            GameEditor->Selected = ECS::INVALID_ENTITY_ID;

        GameEditor->CommandList.AddCommand<DeleteGO>(deletedObjectId);
        GameEditor->CommandList.Redo();

        deletedObjectId = ECS::INVALID_ENTITY_ID;
    }

    if (parentObjectId != ECS::INVALID_ENTITY_ID)
    {
        if (targetObjectId == ECS::INVALID_ENTITY_ID)
            targetObjectId = menu.targetObjectId;

        if (targetObjectId != ECS::INVALID_ENTITY_ID)
        {
            GameEditor->CommandList.AddCommand<AddChild>(targetObjectId, parentObjectId);
            GameEditor->CommandList.Redo();
        }

        parentObjectId = ECS::INVALID_ENTITY_ID;
        targetObjectId = ECS::INVALID_ENTITY_ID;
        GameEditor->Selected = ECS::INVALID_ENTITY_ID;
    }
}

void SceneViewer::recursiveTreeDraw(const std::string &name, GameObject *gameObject)
{
    static ImGuiTreeNodeFlags baseFlags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGuiTreeNodeFlags nodeFlags = baseFlags;

    ECS::EntityId entityId = gameObject->GetEntityID();

    if (gameObject->GetChildCount() == 0 || gameObject->IsPrefab())
        nodeFlags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
    if (GameEditor->Selected == entityId)
        nodeFlags |= ImGuiTreeNodeFlags_Selected;

    if (gameObject->IsPrefab())
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0f, 0.4f, 1.0f, 1.0f));
    bool isSubTreeOpen = ImGui::TreeNodeEx(std::string(name + "##" + std::to_string(entityId)).c_str(), nodeFlags);
    if (gameObject->IsPrefab())
    {
        isSubTreeOpen = false;
        ImGui::PopStyleColor();
    }

    if (ImGui::BeginPopupContextItem(std::string(std::to_string(entityId) + name).c_str()))
    {
        if (ImGui::BeginMenu("Create..."))
        {
            menu.targetObjectId = ECS::INVALID_ENTITY_ID;
            parentObjectId = entityId;
            menu.Draw();
            ImGui::EndMenu();
        }

        if (ImGui::MenuItem("Delete"))
            deletedObjectId = entityId;

        ImGui::EndPopup();
    }

    if (ImGui::IsItemClicked())
        GameEditor->Selected = entityId;

    if (ImGui::BeginDragDropSource())
    {
        ImGui::SetDragDropPayload("SCENE_GO", &entityId, sizeof(ECS::EntityId));
        ImGui::Text("Game object: %s", name.c_str());
        ImGui::EndDragDropSource();
    }

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_GO"))
        {
            targetObjectId = *((ECS::EntityId*)payload->Data);
            parentObjectId = gameObject->GetEntityID();
            ImGui::EndDragDropTarget();
        }
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("ASSET_"))
        {
            std::string_view str = *static_cast<std::string_view*>(payload->Data);
            if (GameEngine->GetAssetsManager().GetAsset<Prefab>(str.data()) != nullptr)
            {
                GameEditor->CommandList.AddCommand<InstantiatePrefab>(std::string(str), &targetObjectId);
                GameEditor->CommandList.Redo();

                parentObjectId = gameObject->GetEntityID();
            }
            ImGui::EndDragDropTarget();
        }
    }

    if (isSubTreeOpen && gameObject->GetChildCount() > 0)
    {
        for (size_t i = 0; i < gameObject->GetChildCount(); ++i)
        {
            auto *childGameObject = static_cast<GameObject*>(gameObject->GetChild(i));
            recursiveTreeDraw(childGameObject->Name, childGameObject);
        }
        ImGui::TreePop();
    }
}

SceneViewer::SceneMenu::SceneMenu()
    : IMenuEntry("Scene")
{}

void SceneViewer::SceneMenu::Draw()
{
    if (RegisterItem("Empty"))
    {
        auto func = [](GameObject* go) {
            go->Name = "Empty";
            go->AddComponent<Transform>();
        };
        GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
        GameEditor->CommandList.Redo();
    }
    if (RegisterItem("Camera"))
    {
        auto func = [](GameObject* go) {
            go->Name = "Camera";
            go->AddComponent<Transform>();
            go->AddComponent<Camera>();
        };
        GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
        GameEditor->CommandList.Redo();
    }
    if (RegisterSubMenu("Solid objects"))
    {
        if (RegisterItem("Plane"))
        {
            auto func = [](GameObject* go) {
                go->Name = "Plane";
                go->AddComponent<Transform>();
                auto *mesh = go->AddComponent<MeshComponent>();
                auto *materialComponent = go->AddComponent<MaterialComponent>();
                mesh->SetMesh(Utils::CreatePlane());
                auto materialHandle = std::make_shared<Material>();
                auto *material = static_cast<Material*>(materialHandle.get());
                material->Shader = "BlinnPhong";
                material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 0.0f, 0.0f)};
                material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
                materialComponent->SetMaterial(materialHandle);
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        if (RegisterItem("Cube"))
        {
            auto func = [](GameObject* go) {
                go->Name = "Cube";
                go->AddComponent<Transform>();
                auto *mesh = go->AddComponent<MeshComponent>();
                auto *materialComponent = go->AddComponent<MaterialComponent>();
                mesh->SetMesh(Utils::CreateCube());
                auto materialHandle = std::make_shared<Material>();
                auto *material = static_cast<Material*>(materialHandle.get());
                material->Shader = "BlinnPhong";
                material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 0.0f, 0.0f)};
                material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
                materialComponent->SetMaterial(materialHandle);
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        if (RegisterItem("Sphere"))
        {
            auto func = [](GameObject* go) {
                go->Name = "Sphere";
                go->AddComponent<Transform>();
                auto *mesh = go->AddComponent<MeshComponent>();
                auto *materialComponent = go->AddComponent<MaterialComponent>();
                mesh->SetMesh(Utils::CreateSphere(32));
                auto materialHandle = std::make_shared<Material>();
                auto *material = static_cast<Material*>(materialHandle.get());
                material->Shader = "BlinnPhong";
                material->Uniforms["color_diffuse"] = {Utils::ShaderValue::Vector3, glm::vec3(1.0f, 0.0f, 0.0f)};
                material->Uniforms["main_specular"] = {Utils::ShaderValue::Float, 0.1f};
                materialComponent->SetMaterial(materialHandle);
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        EndSubMenu();
    }
    if (RegisterSubMenu("Lights"))
    {
        if (RegisterItem("Directional light"))
        {
            auto func = [](GameObject* go) {
                go->Name = "Directional light";
                go->AddComponent<Transform>();
                auto *light = go->AddComponent<LightSource>();
                light->Type = LightType::Directional;
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        if (RegisterItem("Point light"))
        {
            auto func = [](GameObject* go) {
                go->Name = "Point light";
                go->AddComponent<Transform>();
                auto *light = go->AddComponent<LightSource>();
                light->Type = LightType::Point;
                light->Radius = 50;
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        if (RegisterItem("Spot light"))
        {
            auto func = [](GameObject* go){
                go->Name = "Spot light";
                go->AddComponent<Transform>();
                auto *light = go->AddComponent<LightSource>();
                light->Type = LightType::Spot;
                light->Radius = 50;
                light->CutterOff = 12.5f;
            };
            GameEditor->CommandList.AddCommand<CustomCreate>(&targetObjectId, func);
            GameEditor->CommandList.Redo();
        }
        EndSubMenu();
    }
}
