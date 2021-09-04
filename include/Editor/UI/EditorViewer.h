#ifndef EDITORVIEWER_H
#define EDITORVIEWER_H

#include "Components/Camera.h"
#include "Systems/RenderSystem.h"
#include "Editor/UI/IEditorWindow.h"

enum class GizmoOperation : u8 {
    Translation = 0,
    Rotate,
    Scale
};

enum class GizmoMode : u8 {
    Local = 0,
    Global
};

class EditorViewer final : public IEditorWindow {
private:
    bool isFirstClick = true;
    f64 lastMouseX{}, lastMouseY{};
    glm::vec3 cameraAngle{};

    GizmoOperation activeOperation = GizmoOperation::Translation;
    GizmoMode activeMode = GizmoMode::Local;

    u64 lastCommandId = 0;

    void moveCamera(glm::vec4 windowPosition) noexcept;
    void dragCamera(glm::vec4 windowPosition) noexcept;

    void showGizmo(const CameraInfo &cameraInfo, glm::vec4 windowPosition);

public:
    struct EditorCamera {
        f32 fov = 60.0f;
        f32 ratio = 1.5f;
        f32 nearClip = 0.1f;
        f32 farClip = 100.0f;
        glm::vec3 position = glm::vec3(0.0f);
        glm::quat orientation = glm::quat();
        Projection projection = Projection::Perspective;
    };

    EditorCamera Camera;
    f32 CameraSpeed = 5.0f;
    f32 CameraSense = 5.0f;
    bool CameraInverted = false;

    void Draw() final;
};

#endif
