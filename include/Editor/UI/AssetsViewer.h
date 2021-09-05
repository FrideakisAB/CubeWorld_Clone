#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <string>
#include "Editor/UI/IEditorWindow.h"

class AssetsViewer final : public IEditorWindow {
private:
    ImGuiTextFilter filter;
    std::string selected;

public:
    void Draw() final;
};

#endif
