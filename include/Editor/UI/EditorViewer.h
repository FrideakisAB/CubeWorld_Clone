#ifndef EDITORVIEWER_H
#define EDITORVIEWER_H

#include "Components/Camera.h"
#include "Editor/UI/IEditorWindow.h"

class EditorViewer final : public IEditorWindow {
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

    void Draw() final;
};

#endif
