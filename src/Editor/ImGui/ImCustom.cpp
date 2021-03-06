#include "Editor/ImGui/ImCustom.h"

#include <array>
#include <string>
#include "imgui_internal.h"
#include "Render/Texture.h"

void ImGui::TextureWidget(const std::string &asset, ImVec2 size)
{
    if (auto *texture = GameEngine->GetAssetsManager().GetAsset<Texture>(asset); texture != nullptr && texture->GetType() == TexType::Texture2D)
    {
        texture->RenderUpdate();

        if (texture->GetDrawData().Handle != 0)
        {
            if (size.x == 0.0f && size.y == 0.0f)
                size = ImVec2(texture->GetWH().x, texture->GetWH().y);
            else
            {
                if (texture->GetWH().x > texture->GetWH().y)
                {
                    f64 aspect = size.x / static_cast<f64>(texture->GetWH().x);
                    size.y = texture->GetWH().y * aspect;
                }
                else if (texture->GetWH().x < texture->GetWH().y)
                {
                    f64 aspect = size.y / static_cast<f64>(texture->GetWH().y);
                    size.x = texture->GetWH().x * aspect;
                }
            }

            ImGui::Image(reinterpret_cast<void *>(texture->GetDrawData().Handle), size);
        }
    }
}

std::array<ImU32, 4> statesColor = {
        IM_COL32(153, 153, 153, 255),
        IM_COL32(0, 45, 207, 255),
        IM_COL32(78, 204, 0, 255),
        IM_COL32(204, 0, 0, 255),
};

std::array<const char*, 4> statesString = {
        "not have asset",
        "have local asset",
        "have asset",
        "invalid, asset is not valid type"
};

bool ImGui::TextHandleButton(const std::string &label, const std::string &context, const std::string &type, CustomTextState state, u32 height, std::function<void()> dndTarget, std::function<void()> dndSource)
{
    ImVec2 widget_pos = ImGui::GetCursorScreenPos();

    float maxWidth = ImMin(ImGui::GetContentRegionAvailWidth() - 100.0f, ImGui::CalcTextSize(context.c_str()).x + 5.0f);
    bool clicked = ImGui::InvisibleButton(label.c_str(), ImVec2(maxWidth, height));

    if (ImGui::BeginDragDropTarget())
        dndTarget();

    if (ImGui::BeginDragDropSource())
    {
        dndSource();
        ImGui::EndDragDropSource();
    }

    if (ImGui::IsItemHovered())
    {
        ImGui::BeginTooltip();
        ImGui::Text("%s: %s, state: %s", type.c_str(), context.c_str(), statesString[static_cast<unsigned int>(state)]);
        ImGui::End();
    }

    ImDrawList* draw_list = ImGui::GetWindowDrawList();
    draw_list->AddRectFilled(widget_pos, ImVec2(widget_pos.x + maxWidth, widget_pos.y + height), statesColor[static_cast<unsigned int>(state)]);
    draw_list->AddRect(widget_pos, ImVec2(widget_pos.x + maxWidth, widget_pos.y + height), IM_COL32(0, 0, 0, 255));
    draw_list->AddText(ImVec2(widget_pos.x + 2.5f, widget_pos.y), GetColorU32(ImVec4(1.0f, 1.0f, 1.0f, 1.0f)), context.c_str());

    ImGui::SameLine();
    ImGui::Text(label.c_str());

    return clicked;
}