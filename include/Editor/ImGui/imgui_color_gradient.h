#ifndef IMGUI_COLOR_GRADIENT_H
#define IMGUI_COLOR_GRADIENT_H

#include <list>
#include "imgui.h"
#include "Utils/Gradient.h"

enum class ChangeType : u8 {
    Color = 0,
    Position,
    Delete,
    Add,
    None
};

namespace ImGui
{
    bool GradientButton(const char *str_name, Gradient *gradient, u32 height);
    bool GradientEditor(Gradient *gradient);
    bool GradientEditorNoChange(Gradient *gradient, Gradient::MarkIterator &changeItem, ChangeType &changeType, glm::vec4 &color, f32 &position);
    void GradientChange(Gradient *gradient, Gradient::MarkIterator &changeItem, ChangeType &changeType, glm::vec4 &color, f32 &position);
}

#endif