#ifndef EDITORRENDER_H
#define EDITORRENDER_H

#include <vector>
#include "Render/Render.h"
#include "Render/Shader.h"
#include "Editor/UI/EditorViewer.h"
#include "Editor/Render/LinesMesh.h"

class EditorRender {
private:
    struct RenderTask {
        DrawData data;
        glm::mat4 transform;
    };

    Shader lines;
    std::vector<RenderTask> tasks;

public:
    EditorRender();

    void Draw(CameraInfo cameraInfo);

    void AddDrawLines(LinesMesh *linesMesh, const glm::mat4 &transform=glm::mat4(1.0f));
};

#endif
