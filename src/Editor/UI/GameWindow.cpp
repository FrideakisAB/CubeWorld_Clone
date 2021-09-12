#include "Editor/UI/GameWindow.h"

#include "Engine.h"
#include "GameScene.h"
#include "Editor/Editor.h"
#include "ECS/util/Timer.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "Systems/RenderSystem.h"

GameWindow::~GameWindow()
{
    cache = GameEditor->CacheSystem.GetCache(cache.GetID());
    if (!cache.GetPath().empty())
        GameEditor->CacheSystem.RemoveCache(cache);
}

void GameWindow::Draw()
{
    if (ImGui::BeginDock("Game", &Active))
    {
        ImVec2 sizeAvail = ImGui::GetContentRegionAvail();
        ImVec2 position = ImGui::GetWindowPos();

        ImVec2 buttonSize = ImGui::CalcTextSize("Play Pause Stop");
        ImGui::SetCursorPosX((sizeAvail.x - buttonSize.x) / 2);

        if (ImGui::Button("Play"))
        {
            if (state == State::Stop)
            {
                GameEditor->CommandList.InvalidateAll();
                cache = GameEditor->CacheSystem.CreateCache(8);
                std::ofstream file = std::ofstream(fs::current_path().string() + cache.GetPath());
                std::string jsonStr = GameEngine->GetGameScene().SerializeObj().dump(4);
                file.write(jsonStr.c_str(), jsonStr.size());
                file.close();
            }
            else if (state == State::Paused)
                ECS::ECS_Engine->GetTimer()->SetTimeScale(oldScaled);

            state = State::Run;
            GameEditor->IsActiveSimulate = true;
        }

        ImGui::SameLine();
        if (ImGui::Button("Pause"))
        {
            if (state == State::Run)
            {
                oldScaled = ECS::ECS_Engine->GetTimer()->GetTimeScale();
                ECS::ECS_Engine->GetTimer()->SetTimeScale(0.0f);
                state = State::Paused;
            }
        }

        ImGui::SameLine();
        if (ImGui::Button("Stop"))
        {
            if (state != State::Stop)
            {
                if (state == State::Paused)
                    ECS::ECS_Engine->GetTimer()->SetTimeScale(oldScaled);

                GameEditor->Selected = ECS::INVALID_ENTITY_ID;
                GameEditor->CommandList.InvalidateAll();
                cache = GameEditor->CacheSystem.GetCache(cache.GetID());
                if (!cache.GetPath().empty())
                {
                    std::ifstream file = std::ifstream(fs::current_path().string() + cache.GetPath());
                    GameEngine->GetGameScene().UnSerializeObj(json_utils::TryParse(Utils::FileToString(std::move(file))));
                    GameEditor->CacheSystem.RemoveCache(cache);
                }
            }

            state = State::Stop;
            GameEditor->IsActiveSimulate = false;
        }

        sizeAvail = ImGui::GetContentRegionAvail();

        if (sizeAvail.x >= 100.0f && sizeAvail.y >= 100.0f && (state == State::Run || state == State::Paused))
        {
            int width, height;
            glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

            glm::vec4 windowPosition = glm::vec4(position.x + 5.0f, position.y + ImGui::GetCursorPosY() + 5.0f, sizeAvail.x + 10.0f, sizeAvail.y + 10.0f);

            GameEngine->GetRenderSystem().Resize(windowPosition.x, height - windowPosition.y - windowPosition.w, windowPosition.z, windowPosition.w);
            GameEngine->GetRenderSystem().PreUpdate();
            GameEngine->GetRenderSystem().Update();
            GameEngine->GetRenderSystem().PostUpdate();
        }
    }
    ImGui::EndDock();

    if (state == State::Run)
        GameEngine->Update();
}
