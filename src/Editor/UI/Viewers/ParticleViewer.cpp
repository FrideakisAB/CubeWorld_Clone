#include "Editor/UI/Viewers/ParticleViewer.h"

#include "Editor/Editor.h"
#include <glm/gtc/type_ptr.hpp>
#include "Components/ParticleSystem.h"
#include "Editor/Commands/ViewersCommands.h"

void ParticleViewer::OnEditorUI(GameObject &go, ECS::IComponent &cmp)
{
    auto &ps = dynamic_cast<ParticleSystem&>(cmp);
    bool closed = true;
    if (ImGui::CollapsingHeader("Particle system", &closed))
    {
        if (ImGui::TreeNode("Main"))
        {
            bool update = false;

            f32 duration = ps.Duration;
            ImGui::InputFloat("Duration", &duration);
            if (duration != ps.Duration)
                update = true;

            u32 maxPart = ps.GetMaxParticles();
            ImGui::InputScalar("Max particles", ImGuiDataType_U32, &maxPart, nullptr, nullptr, "%u");
            if (maxPart != ps.GetMaxParticles())
                update = true;

            bool loop = ps.Loop;
            ImGui::Checkbox("Loop", &loop);
            if (loop != ps.Loop)
                update = true;

            bool globalSpace = ps.GlobalSpace;
            ImGui::Checkbox("Global space", &globalSpace);
            if (globalSpace != ps.GlobalSpace)
                update = true;

            bool playOnStart = ps.PlayOnStart;
            ImGui::Checkbox("Play on start", &playOnStart);
            if (playOnStart != ps.PlayOnStart)
                update = true;

            f32 startDelay = ps.StartDelay;
            ImGui::InputFloat("Start delay", &startDelay);
            if (startDelay != ps.StartDelay)
                update = true;

            f32 startLifetime = ps.StartLifetime;
            ImGui::InputFloat("Start Lifetime", &startLifetime);
            if (startLifetime != ps.StartLifetime)
                update = true;

            f32 startSpeed = ps.StartSpeed;
            ImGui::InputFloat("Start speed", &startSpeed);
            if (startSpeed != ps.StartSpeed)
                update = true;

            f32 startSize = ps.StartSize;
            ImGui::InputFloat("Start size", &startSize);
            if (startSize != ps.StartSize)
                update = true;

            f32 startRotation = ps.StartRotation;
            ImGui::InputFloat("Start rotation", &startRotation);
            if (startRotation != ps.StartRotation)
                update = true;

            glm::vec4 startColor = ps.StartColor;
            ImGui::ColorEdit4("Start color", glm::value_ptr(startColor), ImGuiColorEditFlags_NoInputs);
            if (startColor != ps.StartColor)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.Duration = duration;
                if (maxPart != ps.GetMaxParticles())
                    ps.SetMaxParticles(maxPart);
                ps.Loop = loop;
                ps.GlobalSpace = globalSpace;
                ps.PlayOnStart = playOnStart;
                ps.StartDelay = startDelay;
                ps.StartLifetime = startLifetime;
                ps.StartSpeed = startSpeed;
                ps.StartSize = startSize;
                ps.StartRotation = startRotation;
                ps.StartColor = startColor;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Emission"))
        {
            bool update = false;

            u32 rate = ps.Emission.Rate;
            ImGui::InputScalar("Rate", ImGuiDataType_U32, &rate, nullptr, nullptr, "%u");
            if (rate != ps.Emission.Rate)
                update = true;

            const char* typeRateStr[] = { "Time", "Distance" };
            int typeRateCurrent = static_cast<int>(ps.Emission.TypeRate);
            ImGui::Combo("Type rate", &typeRateCurrent, typeRateStr, IM_ARRAYSIZE(typeRateStr));
            if (typeRateCurrent != static_cast<int>(ps.Emission.TypeRate))
                update = true;

            const char* shapeStr[] = { "Plane", "Circle", "Sphere", "Hemisphere", "Cone" };
            int shapeCurrent = static_cast<int>(ps.Shape);
            ImGui::Combo("Shape", &shapeCurrent, shapeStr, IM_ARRAYSIZE(shapeStr));
            if (shapeCurrent != static_cast<int>(ps.Shape))
                update = true;

            bool isDelete = false, isAdded = false, isModify = false;
            size_t deleteIndex = 0, modifyIndex = 0;
            Burst modifyBurst{};
            if (ImGui::TreeNode("Bursts"))
            {
                for (size_t i = 0; i < ps.Emission.Bursts.size(); ++i)
                {
                    std::string iString = std::to_string(i);
                    ImGui::Text("Burst %llu", i);
                    ImGui::SameLine();
                    if (ImGui::Button(std::string("Delete##" + iString).c_str()))
                    {
                        update = true;
                        isDelete = true;
                        deleteIndex = i;
                    }

                    modifyBurst = ps.Emission.Bursts[i];

                    ImGui::InputFloat(std::string("Time##" + iString).c_str(), &modifyBurst.Time);
                    ImGui::InputScalar(std::string("Minimum count##" + iString).c_str(), ImGuiDataType_U32, &modifyBurst.MinCount, nullptr, nullptr, "%u");
                    ImGui::InputScalar(std::string("Maximum count##" + iString).c_str(), ImGuiDataType_U32, &modifyBurst.MaxCount, nullptr, nullptr, "%u");
                    ImGui::Text("Deploy: %s", modifyBurst.IsMake? "true" : "false");

                    if (modifyBurst.Time != ps.Emission.Bursts[i].Time ||
                        modifyBurst.MinCount != ps.Emission.Bursts[i].MinCount ||
                        modifyBurst.MaxCount != ps.Emission.Bursts[i].MaxCount)
                    {
                        update = true;
                        isModify = true;
                        modifyIndex = i;
                    }
                }

                if (ImGui::Button("Add burst"))
                {
                    update = true;
                    isAdded = true;
                }

                ImGui::TreePop();
            }

            if (update)
            {
                if (lastCommandIds[1] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[1]))
                    lastCommandIds[1] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.Emission.Rate = rate;
                ps.Emission.TypeRate = static_cast<EmissionType>(typeRateCurrent);
                ps.Shape = static_cast<Emitter>(shapeCurrent);

                if (isDelete)
                    ps.Emission.Bursts.erase(ps.Emission.Bursts.begin() + deleteIndex);

                if (isAdded)
                {
                    Burst burst{};
                    ps.Emission.Bursts.push_back(burst);
                }

                if (isModify)
                    ps.Emission.Bursts[modifyIndex] = modifyBurst;
            }

            ImGui::TreePop();
        }
    }
    if (!closed)
    {
        GameEditor->CommandList.AddCommand<DeleteComponent<ParticleSystem>>(&go);
        GameEditor->CommandList.Redo();
    }
}
