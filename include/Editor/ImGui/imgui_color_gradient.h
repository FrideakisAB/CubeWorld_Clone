#ifndef IMGUI_COLOR_GRADIENT_H
#define IMGUI_COLOR_GRADIENT_H

#include <list>
#include "imgui.h"
#include "Utils/Gradient.h"

namespace ImGui
{
    bool GradientButton(const char* str_name, Gradient* gradient, u32 height);
    bool GradientEditor(Gradient* gradient, Gradient::MarkIterator &draggingMark, Gradient::MarkIterator &selectedMark);
}

#endif