#ifndef IMGUI_CUSTOM_H
#define IMGUI_CUSTOM_H

#include "imgui.h"
#include <glm/glm.hpp>

enum class CustomTextState : unsigned int {
    None = 0,
    NoGlobal,
    Global,
    Invalid
};

namespace ImGui
{
    bool TextHandleButton(const char *label, const char *context, const char *type, CustomTextState state, unsigned int height);
};

#endif
