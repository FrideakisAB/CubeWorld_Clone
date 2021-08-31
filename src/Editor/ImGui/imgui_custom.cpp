#include "Editor/ImGui/imgui_custom.h"

#include <array>
#include <string>
#include "imgui_internal.h"

std::array<ImU32, 4> statesColor = {
        IM_COL32(153, 153, 153, 255),
        IM_COL32(0, 45, 207, 255),
        IM_COL32(78, 204, 0, 255),
        IM_COL32(204, 0, 0, 255),
};

std::array<const char*, 4> statesString = {
        "not have mesh",
        "have local mesh",
        "have asset mesh",
        "invalid, asset is not mesh"
};

bool ImGui::TextHandleButton(const char *label, const char *context, CustomTextState state, unsigned int height)
{
    ImVec2 widget_pos = ImGui::GetCursorScreenPos();

    float maxWidth = ImMin(ImGui::GetContentRegionAvailWidth() - 100.0f, ImGui::CalcTextSize(context).x + 5.0f);
    bool clicked = ImGui::InvisibleButton(label, ImVec2(maxWidth, height));
    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("Mesh: %s, state: %s", context, statesString[static_cast<unsigned int>(state)]);
        ImGui::End();
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();

    draw_list->AddRectFilled(widget_pos, ImVec2(widget_pos.x + maxWidth, widget_pos.y + height), statesColor[static_cast<unsigned int>(state)]);
    draw_list->AddRect(widget_pos, ImVec2(widget_pos.x + maxWidth, widget_pos.y + height), IM_COL32(0, 0, 0, 255));
    draw_list->AddText(ImVec2(widget_pos.x + 2.5f, widget_pos.y), GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), context);

    ImGui::SameLine();
    ImGui::Text(label);

    return clicked;
}