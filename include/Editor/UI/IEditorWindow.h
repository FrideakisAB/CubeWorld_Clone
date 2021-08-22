#ifndef IEDITORWINDOW_H
#define IEDITORWINDOW_H

#include "imgui.h"
#include "Editor/ImGui/imgui_dock.h"

class IMenuEntry;

class IEditorWindow {
public:
    virtual ~IEditorWindow() = default;

    virtual void Draw() = 0;
    virtual IMenuEntry *GetMenuEntry() { return nullptr; }

    bool Active = true;
};

#endif
