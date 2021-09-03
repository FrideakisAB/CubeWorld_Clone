#ifndef LOGVIEWER_H
#define LOGVIEWER_H

#include "Log.h"
#include "Editor/UI/IEditorWindow.h"

class LogViewer final : public IEditorWindow {
    class GraphLog final : public Log {
    private:
        ImGuiTextBuffer buffer;
        ImGuiTextFilter filter;
        ImVector<int> lineOffsets;
        std::mutex lock;

    protected:
        void Write(const std::string &msg) override;

    public:
        GraphLog();

        void Clear();
        void Draw();
    };

private:
    GraphLog graphLog;
    Log *oldLog = nullptr;

public:
    void Draw() final;

    void StartCapture();
    void EndCapture();
};

#endif
