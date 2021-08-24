#include "Editor/UI/SceneEditor.h"

#include "Engine.h"
#include "GameScene.h"
#include "Editor/Editor.h"
#include "Editor/Commands/SceneEditorCommands.h"

void SceneEditor::Draw()
{
    if (ImGui::BeginDock("Scene editor", &Active) && GameEditor->Selected != ECS::INVALID_ENTITY_ID)
    {
        auto *go = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(GameEditor->Selected));

        {
            char buf[256] = {0};
            if(go->Name.size() < 256)
                std::copy(go->Name.begin(), go->Name.end(), buf);
            ImGui::InputText("Name", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_NoUndoRedo);

            if(std::string(buf) != go->Name)
            {
                GameEditor->CommandList.AddCommand<TextEdit>(go, std::string(buf), false);
                GameEditor->CommandList.Redo();
            }
        }
        {
            char buf[256] = {0};
            if(go->Tag.size() < 256)
                std::copy(go->Tag.begin(), go->Tag.end(), buf);
            ImGui::InputText("Tag", buf, IM_ARRAYSIZE(buf), ImGuiInputTextFlags_NoUndoRedo);

            if(std::string(buf) != go->Tag)
            {
                GameEditor->CommandList.AddCommand<TextEdit>(go, std::string(buf), true);
                GameEditor->CommandList.Redo();
            }
        }

        bool active = go->IsLocalActive();
        ImGui::Checkbox("Active", &active);
        if (active != go->IsLocalActive())
        {
            GameEditor->CommandList.AddCommand<ActiveEdit>(go, active);
            GameEditor->CommandList.Redo();
        }

        auto* CM = ECS::ECS_Engine->GetComponentManager();
        const size_t NUM_COMPONENTS = CM->entityComponentMap[0].size();
        for (ECS::ComponentTypeId componentTypeId = 0; componentTypeId < NUM_COMPONENTS; ++componentTypeId)
        {
            const ECS::ComponentId componentId = CM->entityComponentMap[go->GetEntityID().index][componentTypeId];
            if (componentId == ECS::INVALID_COMPONENT_ID)
                continue;

            if (ECS::IComponent* component = CM->componentLut[componentId]; component != nullptr)
            {
                if (IViewer *viewer = ViewersRegistry.GetViewer(componentTypeId); viewer != nullptr)
                    viewer->OnEditorUI(*go, *component);
            }
        }

        ImGui::SetCursorPosX((ImGui::GetWindowWidth() - 100) / 2);
        if (ImGui::Button("Add component"))
            ImGui::OpenPopup("addComponent");

        if (ImGui::BeginPopup("addComponent"))
        {
            ImGui::Text("Components");
            ImGui::Separator();

            for (auto &setCmp : ECS::ECS_Engine->GetComponentFactory()->cmpSetRegistry)
                if (ImGui::Selectable((setCmp.first.substr(setCmp.first.find(' '))).c_str()))
                {
                    GameEditor->CommandList.AddCommand<AddComponent>(go, setCmp.first);
                    GameEditor->CommandList.Redo();
                }

            ImGui::EndPopup();
        }
    }
    ImGui::EndDock();
}
