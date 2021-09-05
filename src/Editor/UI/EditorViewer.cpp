#include "Editor/UI/EditorViewer.h"

#include "Engine.h"
#include "imgui_internal.h"
#include "Render/GLUtils.h"
#include <GLFW/glfw3.h>
#include "Editor/Editor.h"
#include "ECS/util/Timer.h"
#include "Editor/ImGui/ImGuizmo.h"
#include <glm/gtx/euler_angles.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include "Editor/Commands/ViewersCommands.h"
#include "Components/Components.h"

EditorViewer::EditorViewer()
{
    cameraMesh.ReserveVertex(12);
    particleMesh.ReserveVertex(96);
    lightMesh.ReserveVertex(192);
}

void EditorViewer::Draw()
{
    if (ImGui::BeginDock("Editor viewer", &Active))
    {
        ImVec2 sizeAvail = ImGui::GetContentRegionAvail();
        ImVec2 position = ImGui::GetWindowPos();

        if (sizeAvail.x >= 100.0f && sizeAvail.y >= 100.0f)
        {
            ImGui::SetNextItemWidth(110.0f);
            const char* operationsString[] = { "Transform", "Rotate", "Scale" };
            int current = static_cast<u8>(activeOperation);
            ImGui::Combo("##1", &current, operationsString, IM_ARRAYSIZE(operationsString)); ImGui::SameLine();
            activeOperation = static_cast<GizmoOperation>(current);

            ImGui::SetNextItemWidth(80.0f);
            const char* modesString[] = { "Local", "Global" };
            current = static_cast<u8>(activeMode);
            ImGui::Combo("##2", &current, modesString, IM_ARRAYSIZE(modesString));
            activeMode = static_cast<GizmoMode>(current);

            int width, height;
            glfwGetFramebufferSize(glfwGetCurrentContext(), &width, &height);

            glm::vec4 windowPosition = glm::vec4(position.x + 5.0f, position.y + 5.0f, sizeAvail.x + 10.0f, sizeAvail.y + 10.0f);

            moveCamera(windowPosition);
            dragCamera(windowPosition);

            lastAspect = sizeAvail.x / sizeAvail.y;

            CameraInfo cameraInfo = GetCameraInfo();

            showGizmo(cameraInfo, windowPosition);
            showParticleControls(windowPosition);
            editorDrawSelected(windowPosition.z, windowPosition.w);

            GameEngine->GetRenderSystem().SetCustomCameraInfo(cameraInfo);
            GameEngine->GetRenderSystem().Resize(windowPosition.x, height - windowPosition.y - windowPosition.w, windowPosition.z, windowPosition.w);
            GameEngine->GetRenderSystem().PreUpdate();
            GameEngine->GetRenderSystem().Update();
            GameEngine->GetRenderSystem().PostUpdate();

            GameEditor->GetRender().Draw(cameraInfo);
        }
        else
            clearParticleState();
    }
    else
        clearParticleState();
    ImGui::EndDock();
}

void EditorViewer::moveCamera(glm::vec4 windowPosition) noexcept
{
    f64 x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
    if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT) &&
        x >= windowPosition.x && x <= windowPosition.z + windowPosition.x &&
        y >= windowPosition.y && y <= windowPosition.w + windowPosition.y)
    {
        f32 dt = ECS::ECS_Engine->GetTimer()->GetNonScaleDeltaTime();
        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_W))
            EditorCamera.position += EditorCamera.orientation * Transform::WorldFront * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_S))
            EditorCamera.position -= EditorCamera.orientation * Transform::WorldFront * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_A))
            EditorCamera.position += EditorCamera.orientation * Transform::WorldRight * CameraSpeed * dt;

        if (glfwGetKey(glfwGetCurrentContext(), GLFW_KEY_D))
            EditorCamera.position -= EditorCamera.orientation * Transform::WorldRight * CameraSpeed * dt;
    }
}

void EditorViewer::dragCamera(glm::vec4 windowPosition) noexcept
{
    f64 x, y;
    glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);

    if (x >= windowPosition.x && x <= windowPosition.z + windowPosition.x &&
        y >= windowPosition.y && y <= windowPosition.w + windowPosition.y)
    {
        f64 dX = lastMouseX - x;
        f64 dY = lastMouseY - y;

        if (isFirstClick)
        {
            dX = 0;
            dY = 0;
        }

        lastMouseX = x;
        lastMouseY = y;

        if (glfwGetMouseButton(glfwGetCurrentContext(), GLFW_MOUSE_BUTTON_RIGHT))
        {
            f32 dt = ECS::ECS_Engine->GetTimer()->GetNonScaleDeltaTime();

            if (!CameraInverted)
            {
                cameraAngle.x -= dX * CameraSense * dt;
                cameraAngle.y += dY * CameraSense * dt;
            }
            else
            {
                cameraAngle.x += dX * CameraSense * dt;
                cameraAngle.y -= dY * CameraSense * dt;
            }

            EditorCamera.orientation = glm::quat_cast(glm::yawPitchRoll(glm::radians(cameraAngle.x), glm::radians(cameraAngle.y), glm::radians(cameraAngle.z)));

            isFirstClick = false;
        }
        else
            isFirstClick = true;
    }
    else
        isFirstClick = true;
}

void EditorViewer::showGizmo(const CameraInfo &cameraInfo, glm::vec4 windowPosition)
{
    if (GameEditor->Selected != ECS::INVALID_ENTITY_ID)
    {
        auto *go = static_cast<GameObject *>(ECS::ECS_Engine->GetEntityManager()->GetEntity(GameEditor->Selected));
        if (auto *transform = go->GetComponent<Transform>(); transform != nullptr)
        {
            ImGuizmo::OPERATION operation;
            switch (activeOperation)
            {
            case GizmoOperation::Translation:
                operation = ImGuizmo::TRANSLATE;
                break;

            case GizmoOperation::Rotate:
                operation = ImGuizmo::ROTATE;
                break;

            case GizmoOperation::Scale:
                operation = ImGuizmo::SCALE;
                break;
            }

            ImGuizmo::MODE mode;
            switch (activeMode)
            {
            case GizmoMode::Local:
                mode = ImGuizmo::LOCAL;
                break;

            case GizmoMode::Global:
                mode = ImGuizmo::WORLD;
                break;
            }

            ImGuizmo::SetOrthographic(EditorCamera.projection == Projection::Orthographic);
            ImGuizmo::BeginFrame();
            glm::mat4 model = transform->GetMat();
            ImGuizmo::SetRect(windowPosition.x, windowPosition.y, windowPosition.z, windowPosition.w);
            ImGuizmo::Manipulate(glm::value_ptr(cameraInfo.view), glm::value_ptr(cameraInfo.projection), operation, mode, &model[0][0]);

            if (model != transform->GetMat())
            {
                transform->GetMat() = model;
                glm::vec3 sizeScale;
                glm::quat orientation;
                glm::vec3 position;
                glm::vec3 skew;
                glm::vec4 perspective;

                glm::decompose(model, sizeScale, orientation, position, skew, perspective);

                Position pos;
                pos.position = position;
                pos.rotate = orientation;
                pos.scale = sizeScale;

                if (lastCommandId == 0 || !GameEditor->CommandList.IsTimedValid(lastCommandId))
                    lastCommandId = GameEditor->CommandList.AddTimedCommand<ChangeState<Transform>>(go);

                if (transform->IsLocalPosition())
                {
                    transform->SetGlobalPos(pos);
                    transform->SetLocalPos(transform->GetLocalPos());
                }
                else
                    transform->SetGlobalPos(pos);
            }
        }
    }
}

void EditorViewer::showParticleControls(glm::vec4 windowPosition)
{
    if (GameEditor->Selected != ECS::INVALID_ENTITY_ID)
    {
        auto *go = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(GameEditor->Selected));
        if (auto *ps = go->GetComponent<ParticleSystem>(); ps != nullptr)
        {
            if (!GameEditor->IsActiveSimulate)
            {
                isParticleUpdate = true;
                particleEntityId = go->GetEntityID();

                ps->Update();
            }

            const ImGuiWindowFlags windowFlags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
                                                 ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
            const f32 PAD = 10.0f;
            ImVec2 windowPos, windowPivot;
            windowPos.x = windowPosition.x + windowPosition.z - PAD;
            windowPos.y = windowPosition.y + windowPosition.w - PAD;
            windowPivot.x = 1.0f;
            windowPivot.y = 1.0f;

            ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always, windowPivot);
            ImGui::SetNextWindowBgAlpha(0.35f);

            f64 x, y;
            glfwGetCursorPos(glfwGetCurrentContext(), &x, &y);
            if (x >= windowPosition.x + windowPosition.z / 3 && x <= windowPosition.z + windowPosition.x &&
                y >= windowPosition.y + windowPosition.w * 2 / 3 && y <= windowPosition.w + windowPosition.y)
                ImGui::SetNextWindowFocus();
            ImGui::Begin("Particle system control", nullptr, windowFlags);
            ImGui::TextColored(ImVec4(1.0f, 0.72f, 0.02f, 1.0f), "Particle system control");
            ImGui::Text("Active particles: %u", ps->GetActiveParticlesCount());

            if (ImGui::Button("Play"))
                ps->Play();

            ImGui::SameLine();
            if (ImGui::Button("Stop"))
                ps->Stop();

            ImGui::SameLine();
            if (ImGui::Button("Resume"))
                ps->Resume();

            ImGui::SameLine();
            if (ImGui::Button("Restart"))
                ps->Restart();

            ImGui::End();

            return;
        }
    }

    clearParticleState();
}

void EditorViewer::clearParticleState()
{
    if (isParticleUpdate)
    {
        auto *particleEntity = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(particleEntityId));
        if (particleEntity != nullptr)
        {
            auto *ps = particleEntity->GetComponent<ParticleSystem>();
            if (ps != nullptr)
                ps->Restart();
        }
        isParticleUpdate = false;
    }
}

CameraInfo EditorViewer::GetCameraInfo() const noexcept
{
    CameraInfo cameraInfo{};

    glm::vec3 up = EditorCamera.orientation * Transform::WorldUp;
    glm::vec3 forward = EditorCamera.orientation * Transform::WorldFront;
    cameraInfo.view = glm::lookAt(EditorCamera.position, EditorCamera.position + forward, up);
    cameraInfo.position = EditorCamera.position;

    if (EditorCamera.projection == Projection::Perspective)
        cameraInfo.projection = glm::perspective(glm::radians(EditorCamera.fov), lastAspect, EditorCamera.nearClip, EditorCamera.farClip);
    else
        cameraInfo.projection = glm::ortho(0.0f, EditorCamera.ratio * lastAspect, 0.0f, EditorCamera.ratio * lastAspect, EditorCamera.nearClip, EditorCamera.farClip);

    return cameraInfo;
}

void EditorViewer::editorDrawSelected(u32 width, u32 height)
{
    if (GameEditor->Selected != ECS::INVALID_ENTITY_ID)
    {
        auto *entity = static_cast<GameObject*>(ECS::ECS_Engine->GetEntityManager()->GetEntity(GameEditor->Selected));
        if (auto *camera = entity->GetComponent<Camera>(); camera != nullptr)
        {
            glm::vec3 cameraColor = glm::vec3(1.0f, 0.0f, 0.0f);

            cameraMesh.ResetPrimitives();
            glm::mat4 inv = glm::inverse(camera->GetVPMatrix(width, height));

            static const glm::vec4 faces[8] = {
                    // near face
                    {1, 1, -1, 1.f},
                    {-1, 1, -1, 1.f},
                    {1, -1, -1, 1.f},
                    {-1, -1, -1, 1.f},

                    // far face
                    {1, 1, 1, 1.f},
                    {-1, 1, 1 , 1.f},
                    {1, -1, 1 , 1.f},
                    {-1, -1,1, 1.f},
            };

            glm::vec3 verts[8];
            for (u32 i = 0; i < 8; ++i)
            {
                glm::vec4 formedFace = inv * faces[i];
                verts[i].x = formedFace.x / formedFace.w;
                verts[i].y = formedFace.y / formedFace.w;
                verts[i].z = formedFace.z / formedFace.w;
            }

            cameraMesh.AddLine({{verts[0].x, verts[0].y, verts[0].z}, {verts[1].x, verts[1].y, verts[1].z}, cameraColor});
            cameraMesh.AddLine({{verts[0].x, verts[0].y, verts[0].z}, {verts[2].x, verts[2].y, verts[2].z}, cameraColor});
            cameraMesh.AddLine({{verts[3].x, verts[3].y, verts[3].z}, {verts[1].x, verts[1].y, verts[1].z}, cameraColor});
            cameraMesh.AddLine({{verts[3].x, verts[3].y, verts[3].z}, {verts[2].x, verts[2].y, verts[2].z}, cameraColor});

            cameraMesh.AddLine({{verts[4].x, verts[4].y, verts[4].z}, {verts[5].x, verts[5].y, verts[5].z}, cameraColor});
            cameraMesh.AddLine({{verts[4].x, verts[4].y, verts[4].z}, {verts[6].x, verts[6].y, verts[6].z}, cameraColor});
            cameraMesh.AddLine({{verts[7].x, verts[7].y, verts[7].z}, {verts[5].x, verts[5].y, verts[5].z}, cameraColor});
            cameraMesh.AddLine({{verts[7].x, verts[7].y, verts[7].z}, {verts[6].x, verts[6].y, verts[6].z}, cameraColor});

            cameraMesh.AddLine({{verts[0].x, verts[0].y, verts[0].z}, {verts[4].x, verts[4].y, verts[4].z}, cameraColor});
            cameraMesh.AddLine({{verts[1].x, verts[1].y, verts[1].z}, {verts[5].x, verts[5].y, verts[5].z}, cameraColor});
            cameraMesh.AddLine({{verts[3].x, verts[3].y, verts[3].z}, {verts[7].x, verts[7].y, verts[7].z}, cameraColor});
            cameraMesh.AddLine({{verts[2].x, verts[2].y, verts[2].z}, {verts[6].x, verts[6].y, verts[6].z}, cameraColor});
            cameraMesh.Apply();

            GameEditor->GetRender().AddDrawLines(&cameraMesh);
        }

        if (auto *transform = entity->GetComponent<Transform>(); transform != nullptr)
        {
            if (auto *ps = entity->GetComponent<ParticleSystem>(); ps != nullptr)
            {
                glm::vec3 particleColor = glm::vec3(0.0f, 1.0f, 0.0f);

                particleMesh.ResetPrimitives();
                glm::mat4 trans;
                switch (ps->Shape)
                {
                case Emitter::Plane:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    particleMesh.AddPlane(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, particleColor, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    break;

                case Emitter::Circle:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    particleMesh.AddCircle(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, particleColor, 32, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    break;

                case Emitter::Sphere:
                    particleMesh.AddSphere(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, particleColor, 32);
                    break;

                case Emitter::Hemisphere:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    particleMesh.AddCircle(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, particleColor, 32, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
                    particleMesh.AddArc(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 180.0f, particleColor, 16);
                    particleMesh.AddArc(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, 180.0f, particleColor, 16, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    break;

                case Emitter::Cone:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    particleMesh.AddCircle(glm::vec3(0.0f, 0.0f, 0.0f), 1.0f, particleColor, 32, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    particleMesh.AddCircle(glm::vec3(0.0f, 1.0f, 0.0f), 1.5f, particleColor, 32, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    particleMesh.AddLine({glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(1.5f, 1.0f, 0.0f), particleColor});
                    particleMesh.AddLine({glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(-1.5f, 1.0f, 0.0f), particleColor});
                    particleMesh.AddLine({glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, 1.0f, 1.5f), particleColor});
                    particleMesh.AddLine({glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, -1.5f), particleColor});
                    break;
                }
                particleMesh.Apply();

                GameEditor->GetRender().AddDrawLines(&particleMesh, transform->GetMat());
            }

            if (auto *light = entity->GetComponent<LightSource>(); light != nullptr)
            {
                glm::vec3 lightColor = glm::vec3(0.96, 0.86, 0);

                lightMesh.ResetPrimitives();
                glm::mat4 trans;
                f32 radius;
                switch(light->Type)
                {
                case LightType::Directional:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    lightMesh.AddLine({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -2.0f, 0.0f), lightColor});
                    lightMesh.AddCircle(glm::vec3(0.0f, 0.0f, 0.0f), 1.5f, lightColor, 48, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    break;

                case LightType::Spot:
                    trans = glm::mat4(1.0f);
                    trans = glm::rotate(trans, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
                    radius = glm::tan(glm::radians(light->CutterOff)) * light->Radius;
                    lightMesh.AddLine({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(radius, -light->Radius, 0.0f), lightColor});
                    lightMesh.AddLine({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-radius, -light->Radius, 0.0f), lightColor});
                    lightMesh.AddLine({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -light->Radius, radius), lightColor});
                    lightMesh.AddLine({glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -light->Radius, -radius), lightColor});
                    lightMesh.AddCircle(glm::vec3(0.0f, -light->Radius, 0.0f), radius, lightColor, 48, glm::mat3(glm::vec3(trans[0]), glm::vec3(trans[1]), glm::vec3(trans[2])));
                    break;

                case LightType::Point:
                    lightMesh.AddSphere(glm::vec3(0.0f, 0.0f, 0.0f), light->Radius, lightColor, 64);
                    break;
                }
                lightMesh.Apply();

                GameEditor->GetRender().AddDrawLines(&lightMesh, transform->GetMat());
            }
        }
    }
}
