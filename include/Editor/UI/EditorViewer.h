#ifndef EDITORVIEWER_H
#define EDITORVIEWER_H

#include "Components/Camera.h"
#include "Systems/RenderSystem.h"
#include "Editor/UI/IEditorWindow.h"
#include "Editor/Render/LinesMesh.h"
#include "Components/ParticleSystem.h"

enum class GizmoOperation : u8 {
    Translation = 0,
    Rotate,
    Scale
};

enum class GizmoMode : u8 {
    Local = 0,
    Global
};

class GameObject;

class EditorViewer final : public IEditorWindow {
private:
    LinesMesh cameraMesh, particleMesh, lightMesh;

    bool isFirstClick = true;
    f64 lastMouseX{}, lastMouseY{};
    glm::vec3 cameraAngle{};

    GizmoOperation activeOperation = GizmoOperation::Translation;
    GizmoMode activeMode = GizmoMode::Local;

    u64 lastCommandId = 0;

    bool isParticleUpdate = false;
    ECS::EntityId particleEntityId = ECS::INVALID_ENTITY_ID;

    f32 lastAspect = 1.0f;

    void moveCamera(glm::vec4 windowPosition) noexcept;
    void dragCamera(glm::vec4 windowPosition) noexcept;

    void showGizmo(const CameraInfo &cameraInfo, glm::vec4 windowPosition);
    void showParticleControls(glm::vec4 windowPosition);
    void clearParticleState();
    void editorDrawSelected(u32 width, u32 height);

public:
    EditorViewer();

    struct EditorCamera {
        f32 fov = 60.0f;
        f32 ratio = 1.5f;
        f32 nearClip = 0.1f;
        f32 farClip = 10000.0f;
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat orientation = glm::quat();
        Projection projection = Projection::Perspective;
    };

    EditorCamera EditorCamera;
    f32 CameraSpeed = 5.0f;
    f32 CameraSense = 5.0f;
    bool CameraInverted = false;

    void Draw() final;

    [[nodiscard]] CameraInfo GetCameraInfo() const noexcept;
};

#endif
