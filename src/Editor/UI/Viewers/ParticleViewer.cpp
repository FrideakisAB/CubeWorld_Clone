#include "Editor/UI/Viewers/ParticleViewer.h"

#include "Editor/Editor.h"
#include <glm/gtc/type_ptr.hpp>
#include "Editor/ImGui/ImCurve.h"
#include "Components/ParticleSystem.h"
#include "Editor/Commands/ViewersCommands.h"
#include "Editor/ImGui/ImColorGradient.h"

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

        if (ImGui::TreeNode("Color over lifetime"))
        {
            bool update = false;

            bool active = ps.ColorOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.ColorOverLifetime.Active)
                update = true;

            if (ImGui::GradientButton("Color", &ps.ColorOverLifetime.Gradient, 12))
                ImGui::OpenPopup("GradientEditor");

            Gradient::MarkIterator changeItem;
            ChangeType changeType;
            glm::vec4 color;
            f32 position;
            if (ImGui::BeginPopup("GradientEditor"))
            {
                update = ImGui::GradientEditorNoChange(&ps.ColorOverLifetime.Gradient, changeItem, changeType, color, position);

                ImGui::EndPopup();
            }

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.ColorOverLifetime.Active = active;
                ImGui::GradientChange(&ps.ColorOverLifetime.Gradient, changeItem, changeType, color, position);
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Size over lifetime"))
        {
            bool update = false;

            bool active = ps.SizeOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.SizeOverLifetime.Active)
                update = true;

            if (ImGui::CurveButton("Size", 16, ps.SizeOverLifetime.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cachedCurve(ps.SizeOverLifetime.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                if (ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cachedCurve))
                    update = true;

                ImGui::EndPopup();
            }

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.SizeOverLifetime.Active = active;
                ps.SizeOverLifetime.Curve = cachedCurve;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Speed over lifetime"))
        {
            bool update = false;

            bool active = ps.SpeedOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.SpeedOverLifetime.Active)
                update = true;

            if (ImGui::CurveButton("Speed", 16, ps.SpeedOverLifetime.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cachedCurve(ps.SpeedOverLifetime.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cachedCurve);

                ImGui::EndPopup();
            }

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.SpeedOverLifetime.Active = active;
                ps.SpeedOverLifetime.Curve = cachedCurve;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation over lifetime"))
        {
            bool update = false;

            bool active = ps.RotationOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.RotationOverLifetime.Active)
                update = true;

            f32 speed = ps.RotationOverLifetime.Speed;
            ImGui::InputFloat("Start rotation", &speed);
            if (speed != ps.RotationOverLifetime.Speed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.RotationOverLifetime.Active = active;
                ps.RotationOverLifetime.Speed = speed;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Force over lifetime"))
        {
            bool update = false;

            bool active = ps.ForceOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.ForceOverLifetime.Active)
                update = true;

            if (ImGui::CurveButton("X", 16, ps.ForceOverLifetime.XCurve))
                ImGui::OpenPopup("CurveEditorX");

            Curve cashedXCurve(ps.ForceOverLifetime.XCurve);
            if (ImGui::BeginPopup("CurveEditorX"))
            {
                update = ImGui::CurveEditor("CurveX", ImVec2(600, 200), cashedXCurve);

                ImGui::EndPopup();
            }

            if (ImGui::CurveButton("Y", 16, ps.ForceOverLifetime.YCurve))
                ImGui::OpenPopup("CurveEditorY");

            Curve cashedYCurve(ps.ForceOverLifetime.YCurve);
            if (ImGui::BeginPopup("CurveEditorY"))
            {
                update = ImGui::CurveEditor("CurveY", ImVec2(600, 200), cashedYCurve);

                ImGui::EndPopup();
            }

            if (ImGui::CurveButton("Z", 16, ps.ForceOverLifetime.ZCurve))
                ImGui::OpenPopup("CurveEditorZ");

            Curve cashedZCurve(ps.ForceOverLifetime.ZCurve);
            if (ImGui::BeginPopup("CurveEditorZ"))
            {
                update = ImGui::CurveEditor("CurveZ", ImVec2(600, 200), cashedZCurve);

                ImGui::EndPopup();
            }

            glm::vec3 baseForce = ps.ForceOverLifetime.BaseForce;
            ImGui::InputFloat3("Base force", glm::value_ptr(baseForce));
            if (baseForce != ps.ForceOverLifetime.BaseForce)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.ForceOverLifetime.Active = active;
                ps.ForceOverLifetime.XCurve = cashedXCurve;
                ps.ForceOverLifetime.YCurve = cashedYCurve;
                ps.ForceOverLifetime.ZCurve = cashedZCurve;
                ps.ForceOverLifetime.BaseForce = baseForce;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Bright over lifetime"))
        {
            bool update = false;

            bool active = ps.BrightOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.BrightOverLifetime.Active)
                update = true;

            if (ImGui::CurveButton("Bright", 16, ps.BrightOverLifetime.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cashedCurve(ps.BrightOverLifetime.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cashedCurve);

                ImGui::EndPopup();
            }

            f32 bright = ps.BrightOverLifetime.BaseBright;
            ImGui::InputFloat("Base bright", &bright);
            if (bright != ps.BrightOverLifetime.BaseBright)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.BrightOverLifetime.Active = active;
                ps.BrightOverLifetime.Curve = cashedCurve;
                ps.BrightOverLifetime.BaseBright = bright;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Texture over lifetime"))
        {
            bool update = false;

            bool active = ps.TextureOverLifetime.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.TextureOverLifetime.Active)
                update = true;

            if (ImGui::CurveButton("Texture", 16, ps.TextureOverLifetime.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cashedCurve(ps.TextureOverLifetime.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cashedCurve);

                ImGui::EndPopup();
            }

            f32 baseTexture = ps.TextureOverLifetime.BaseTexture;
            ImGui::InputFloat("Base texture", &baseTexture);
            if (baseTexture != ps.TextureOverLifetime.BaseTexture)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.TextureOverLifetime.Active = active;
                ps.TextureOverLifetime.Curve = cashedCurve;
                ps.TextureOverLifetime.BaseTexture = baseTexture;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Color by speed"))
        {
            bool update = false;

            bool active = ps.ColorBySpeed.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.ColorBySpeed.Active)
                update = true;

            if (ImGui::GradientButton("Color", &ps.ColorBySpeed.Gradient, 12))
                ImGui::OpenPopup("GradientEditor");

            Gradient::MarkIterator changeItem;
            ChangeType changeType;
            glm::vec4 color;
            f32 position;
            if (ImGui::BeginPopup("GradientEditor"))
            {
                update = ImGui::GradientEditorNoChange(&ps.ColorBySpeed.Gradient, changeItem, changeType, color, position);

                ImGui::EndPopup();
            }

            f32 minSpeed = ps.ColorBySpeed.MinSpeed;
            ImGui::InputFloat("Minimum speed", &minSpeed);
            if (minSpeed != ps.ColorBySpeed.MinSpeed)
                update = true;

            f32 maxSpeed = ps.ColorBySpeed.MaxSpeed;
            ImGui::InputFloat("Maximum speed", &maxSpeed);
            if (maxSpeed != ps.ColorBySpeed.MaxSpeed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.ColorBySpeed.Active = active;
                ImGui::GradientChange(&ps.ColorBySpeed.Gradient, changeItem, changeType, color, position);
                ps.ColorBySpeed.MinSpeed = minSpeed;
                ps.ColorBySpeed.MaxSpeed = maxSpeed;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Rotation by speed"))
        {
            bool update = false;

            bool active = ps.RotationBySpeed.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.RotationBySpeed.Active)
                update = true;

            f32 baseSpeed = ps.RotationBySpeed.BaseSpeed;
            ImGui::InputFloat("Base speed", &baseSpeed);
            if (baseSpeed != ps.RotationBySpeed.BaseSpeed)
                update = true;

            f32 minSpeed = ps.RotationBySpeed.MinSpeed;
            ImGui::InputFloat("Minimum speed", &minSpeed);
            if (minSpeed != ps.RotationBySpeed.MinSpeed)
                update = true;

            f32 maxSpeed = ps.RotationBySpeed.MaxSpeed;
            ImGui::InputFloat("Maximum speed", &maxSpeed);
            if (maxSpeed != ps.RotationBySpeed.MaxSpeed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.RotationBySpeed.Active = active;
                ps.RotationBySpeed.BaseSpeed = baseSpeed;
                ps.RotationBySpeed.MinSpeed = minSpeed;
                ps.RotationBySpeed.MaxSpeed = maxSpeed;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Size by speed"))
        {
            bool update = false;

            bool active = ps.SizeBySpeed.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.SizeBySpeed.Active)
                update = true;

            if (ImGui::CurveButton("Size", 16, ps.SizeBySpeed.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cashedCurve(ps.SizeBySpeed.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cashedCurve);

                ImGui::EndPopup();
            }

            f32 baseSize = ps.SizeBySpeed.BaseSize;
            ImGui::InputFloat("Base size", &baseSize);
            if (baseSize != ps.SizeBySpeed.BaseSize)
                update = true;

            f32 minSpeed = ps.SizeBySpeed.MinSpeed;
            ImGui::InputFloat("Minimum speed", &minSpeed);
            if (minSpeed != ps.SizeBySpeed.MinSpeed)
                update = true;

            f32 maxSpeed = ps.SizeBySpeed.MaxSpeed;
            ImGui::InputFloat("Maximum speed", &maxSpeed);
            if (maxSpeed != ps.SizeBySpeed.MaxSpeed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.SizeBySpeed.Active = active;
                ps.SizeBySpeed.BaseSize = baseSize;
                ps.SizeBySpeed.MinSpeed = minSpeed;
                ps.SizeBySpeed.MaxSpeed = maxSpeed;
                ps.SizeBySpeed.Curve = cashedCurve;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Bright by speed"))
        {
            bool update = false;

            bool active = ps.BrightBySpeed.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.BrightBySpeed.Active)
                update = true;

            if (ImGui::CurveButton("Bright", 16, ps.BrightBySpeed.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cashedCurve(ps.BrightBySpeed.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cashedCurve);

                ImGui::EndPopup();
            }

            f32 baseBright = ps.BrightBySpeed.BaseBright;
            ImGui::InputFloat("Base bright", &baseBright);
            if (baseBright != ps.BrightBySpeed.BaseBright)
                update = true;

            f32 minSpeed = ps.BrightBySpeed.MinSpeed;
            ImGui::InputFloat("Minimum speed", &minSpeed);
            if (minSpeed != ps.BrightBySpeed.MinSpeed)
                update = true;

            f32 maxSpeed = ps.BrightBySpeed.MaxSpeed;
            ImGui::InputFloat("Maximum speed", &maxSpeed);
            if (maxSpeed != ps.BrightBySpeed.MaxSpeed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.BrightBySpeed.Active = active;
                ps.BrightBySpeed.BaseBright = baseBright;
                ps.BrightBySpeed.MinSpeed = minSpeed;
                ps.BrightBySpeed.MaxSpeed = maxSpeed;
                ps.BrightBySpeed.Curve = cashedCurve;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Texture by speed"))
        {
            bool update = false;

            bool active = ps.TextureBySpeed.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.TextureBySpeed.Active)
                update = true;

            if (ImGui::CurveButton("Texture", 16, ps.TextureBySpeed.Curve))
                ImGui::OpenPopup("CurveEditor");

            Curve cashedCurve(ps.TextureBySpeed.Curve);
            if (ImGui::BeginPopup("CurveEditor"))
            {
                update = ImGui::CurveEditor("CurveEditor", ImVec2(600, 200), cashedCurve);

                ImGui::EndPopup();
            }

            f32 baseTexture = ps.TextureBySpeed.BaseTexture;
            ImGui::InputFloat("Base texture", &baseTexture);
            if (baseTexture != ps.TextureBySpeed.BaseTexture)
                update = true;

            f32 minSpeed = ps.TextureBySpeed.MinSpeed;
            ImGui::InputFloat("Minimum speed", &minSpeed);
            if (minSpeed != ps.TextureBySpeed.MinSpeed)
                update = true;

            f32 maxSpeed = ps.TextureBySpeed.MaxSpeed;
            ImGui::InputFloat("Maximum speed", &maxSpeed);
            if (maxSpeed != ps.TextureBySpeed.MaxSpeed)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.TextureBySpeed.Active = active;
                ps.TextureBySpeed.BaseTexture = baseTexture;
                ps.TextureBySpeed.MinSpeed = minSpeed;
                ps.TextureBySpeed.MaxSpeed = maxSpeed;
                ps.TextureBySpeed.Curve = cashedCurve;
            }

            ImGui::TreePop();
        }

        if (ImGui::TreeNode("Particle texture"))
        {
            bool update = false;

            bool active = ps.ParticleTexture.Active;
            ImGui::Checkbox("Active", &active);
            if (active != ps.ParticleTexture.Active)
                update = true;

            glm::uvec2 tiles = ps.ParticleTexture.Tiles;
            ImGui::InputScalarN("Tiles", ImGuiDataType_U32, &tiles[0], 2, nullptr, nullptr, "%u");
            if (tiles != ps.ParticleTexture.Tiles)
                update = true;

            if (update)
            {
                if (lastCommandIds[0] == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandIds[0]))
                    lastCommandIds[0] = GameEditor->CommandList.AddTimedCommand<ChangeState<ParticleSystem>>(&go);

                ps.ParticleTexture.Active = active;
                ps.ParticleTexture.Tiles = tiles;
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
