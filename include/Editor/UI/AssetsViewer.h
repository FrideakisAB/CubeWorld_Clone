#ifndef ASSETSVIEWER_H
#define ASSETSVIEWER_H

#include <string>
#include <vector>
#include "Editor/UI/IEditorWindow.h"

class AssetsViewer final : public IEditorWindow {
private:
    ImGuiTextFilter filter;
    std::string selected;
    std::vector<const char*> assetsNames;
    int itemCurrent = 0;

public:
    void Draw() final;
};

#endif
