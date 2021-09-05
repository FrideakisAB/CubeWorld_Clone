#include "Editor/Render/EditorRender.h"

EditorRender::EditorRender()
{
    auto lineShaderFile = std::ifstream(R"(editorData\shaders\line.json)");
    if (lineShaderFile.is_open())
    {
        json j = json_utils::TryParse(Utils::FileToString(std::move(lineShaderFile)));

        if (j["tag"] == "SHADER")
            lines.UnSerializeObj(j);
    }
}

void EditorRender::Draw(CameraInfo cameraInfo)
{
    if (!tasks.empty())
    {
        glDisable(GL_DEPTH_TEST);
        lines.Use();
        glm::mat4 vp = cameraInfo.projection * cameraInfo.view;
        for (auto &task: tasks)
        {
            lines.SetMat4("mvp", vp * task.transform);
            glBindVertexArray(task.data.VAO);
            glDrawArrays(Utils::GetDrawModeGL(task.data.Mode), 0, task.data.Count * 2);
        }
        glEnable(GL_DEPTH_TEST);
        tasks.clear();
    }
}

void EditorRender::AddDrawLines(LinesMesh *linesMesh, const glm::mat4 &transform)
{
    linesMesh->RenderUpdate();
    tasks.push_back({linesMesh->GetDrawData(), transform});
}
