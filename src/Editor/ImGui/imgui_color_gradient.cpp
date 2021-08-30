#include "Editor/ImGui/imgui_color_gradient.h"

#include <optional>
#include "imgui_internal.h"

static const float GRADIENT_BAR_WIDGET_HEIGHT = 12;
static const float GRADIENT_BAR_EDITOR_HEIGHT = 40;
static const float GRADIENT_MARK_DELETE_DIFFY = 40;

namespace ImGui
{
    static void DrawGradientBar(Gradient* gradient,
                                struct ImVec2 const & bar_pos,
                                float maxWidth,
                                float height)
    {
        ImVec4 colorA = {1,1,1,1};
        ImVec4 colorB = {1,1,1,1};
        float prevX = bar_pos.x;
        float barBottom = bar_pos.y + height;
        std::optional<Gradient::Mark> prevMark = std::nullopt;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        
        draw_list->AddRectFilled(ImVec2(bar_pos.x - 2, bar_pos.y - 2),
                                 ImVec2(bar_pos.x + maxWidth + 2, barBottom + 2),
                                 IM_COL32(100, 100, 100, 255));
        
        if (gradient->GetMarks().empty())
        {
            draw_list->AddRectFilled(ImVec2(bar_pos.x, bar_pos.y),
                                     ImVec2(bar_pos.x + maxWidth, barBottom),
                                     IM_COL32(255, 255, 255, 255));
        }
        
        ImU32 colorAU32 = 0;
        ImU32 colorBU32 = 0;
        
        for (auto mark : gradient->GetMarks())
        {
            float from = prevX;
            float to = prevX = bar_pos.x + mark.position * maxWidth;
            
            if (!prevMark)
            {
                colorA.x = mark.color[0];
                colorA.y = mark.color[1];
                colorA.z = mark.color[2];
				colorA.w = mark.color[3];
            }
            else
            {
                colorA.x = prevMark->color[0];
                colorA.y = prevMark->color[1];
                colorA.z = prevMark->color[2];
				colorA.w = prevMark->color[3];
            }
            
            colorB.x = mark.color[0];
            colorB.y = mark.color[1];
            colorB.z = mark.color[2];
			colorB.w = mark.color[3];
            
            colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
            colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);
            
            if (mark.position > 0.0)
            {
                draw_list->AddRectFilledMultiColor(ImVec2(from, bar_pos.y),
                                                   ImVec2(to, barBottom),
                                                   colorAU32, colorBU32, colorBU32, colorAU32);
            }
            
            prevMark = mark;
        }
        
        if (prevMark && prevMark->position < 1.0)
        {
            draw_list->AddRectFilledMultiColor(ImVec2(prevX, bar_pos.y),
                                               ImVec2(bar_pos.x + maxWidth, barBottom),
                                               colorBU32, colorBU32, colorBU32, colorBU32);
        }
        
        ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 10.0f));
    }
    
    static void DrawGradientMarks(Gradient* gradient,
                                  Gradient::MarkIterator &draggingMark,
                                  Gradient::MarkIterator &selectedMark,
                                  struct ImVec2 const & bar_pos,
                                  float maxWidth,
                                  float height)
    {
        ImVec4 colorA = {1,1,1,1};
        ImVec4 colorB = {1,1,1,1};
        float barBottom = bar_pos.y + height;
        std::optional<Gradient::Mark> prevMark = std::nullopt;
        ImDrawList* draw_list = ImGui::GetWindowDrawList();
        ImU32 colorAU32 = 0;
        ImU32 colorBU32 = 0;
        auto delMark = gradient->GetMarks().end();
        
        for (auto markIt = gradient->GetMarks().begin(); markIt != gradient->GetMarks().end(); ++markIt)
        {
            Gradient::Mark mark = *markIt;
            
            if (selectedMark == gradient->GetMarks().end())
                selectedMark = markIt;
            
            float to = bar_pos.x + mark.position * maxWidth;
            
            if (!prevMark)
            {
                colorA.x = mark.color[0];
                colorA.y = mark.color[1];
                colorA.z = mark.color[2];
				colorA.w = mark.color[3];
            }
            else
            {
                colorA.x = prevMark->color[0];
                colorA.y = prevMark->color[1];
                colorA.z = prevMark->color[2];
                colorA.w = prevMark->color[3];
            }
            
            colorB.x = mark.color[0];
            colorB.y = mark.color[1];
            colorB.z = mark.color[2];
            colorB.w = mark.color[3];
            
            colorAU32 = ImGui::ColorConvertFloat4ToU32(colorA);
            colorBU32 = ImGui::ColorConvertFloat4ToU32(colorB);
            
            draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 6)),
                                         ImVec2(to - 6, barBottom),
                                         ImVec2(to + 6, barBottom), IM_COL32(100, 100, 100, 255));
            
            draw_list->AddRectFilled(ImVec2(to - 6, barBottom),
                                     ImVec2(to + 6, bar_pos.y + (height + 12)),
                                     IM_COL32(100, 100, 100, 255), 1.0f);
            
            draw_list->AddRectFilled(ImVec2(to - 5, bar_pos.y + (height + 1)),
                                     ImVec2(to + 5, bar_pos.y + (height + 11)),
                                     IM_COL32(0, 0, 0, 255), 1.0f);
            
            if (selectedMark == markIt)
            {
                draw_list->AddTriangleFilled(ImVec2(to, bar_pos.y + (height - 3)),
                                             ImVec2(to - 4, barBottom + 1),
                                             ImVec2(to + 4, barBottom + 1), IM_COL32(0, 255, 0, 255));
                
                draw_list->AddRect(ImVec2(to - 5, bar_pos.y + (height + 1)),
                                   ImVec2(to + 5, bar_pos.y + (height + 11)),
                                   IM_COL32(0, 255, 0, 255), 1.0f);
            }
            
            draw_list->AddRectFilledMultiColor(ImVec2(to - 3, bar_pos.y + (height + 3)),
                                               ImVec2(to + 3, bar_pos.y + (height + 9)),
                                               colorBU32, colorBU32, colorBU32, colorBU32);
            
            ImGui::SetCursorScreenPos(ImVec2(to - 6, barBottom));
            ImGui::InvisibleButton("mark", ImVec2(12, 12));
            
            if (ImGui::IsItemHovered())
            {
                if (ImGui::IsMouseClicked(0))
                {
                    selectedMark = markIt;
                    draggingMark = markIt;
                }
                else if (selectedMark != gradient->GetMarks().end() && ImGui::IsMouseClicked(1))
					delMark = selectedMark;
            }
            
            prevMark = mark;
        }
        
        if (delMark != gradient->GetMarks().end())
			gradient->RemoveMark(delMark);
        ImGui::SetCursorScreenPos(ImVec2(bar_pos.x, bar_pos.y + height + 20.0f));
    }
    
    bool GradientButton(const char* str_name, Gradient* gradient, u32 height=GRADIENT_BAR_WIDGET_HEIGHT)
    {
        if (!gradient)
            return false;
        
        ImVec2 widget_pos = ImGui::GetCursorScreenPos();
        
        float maxWidth = ImMax(250.0f, ImGui::GetContentRegionAvailWidth() - 100.0f);
        bool clicked = ImGui::InvisibleButton(str_name, ImVec2(maxWidth, height));
        
        DrawGradientBar(gradient, widget_pos, maxWidth, height);
        ImGui::SameLine();
		ImGui::Text(str_name);
		
        return clicked;
    }
    
    bool GradientEditor(Gradient* gradient)
    {
        if (!gradient)
            return false;
        
        bool modified = false;
        
        ImVec2 bar_pos = ImGui::GetCursorScreenPos();
        bar_pos.x += 10;
        float maxWidth = ImGui::GetContentRegionAvailWidth() - 20;
        float barBottom = bar_pos.y + GRADIENT_BAR_EDITOR_HEIGHT;
        
        ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));
        
        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            float pos = (ImGui::GetIO().MousePos.x - bar_pos.x) / maxWidth;
            
            glm::vec4 color = gradient->GetColorAt(pos);

            gradient->AddMark(pos, color);
        }
        
        DrawGradientBar(gradient, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
        DrawGradientMarks(gradient, gradient->DraggingIterator, gradient->SelectedIterator, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
        
        if (!ImGui::IsMouseDown(0) && gradient->DraggingIterator != gradient->GetMarks().end())
            gradient->DraggingIterator = gradient->GetMarks().end();
        
        if (ImGui::IsMouseDragging(0) && gradient->DraggingIterator != gradient->GetMarks().end())
        {
            float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
            bool insideZone = (ImGui::GetIO().MousePos.x > bar_pos.x) &&
                              (ImGui::GetIO().MousePos.x < bar_pos.x + maxWidth);
            
            if (increment != 0.0f && insideZone)
            {
                gradient->DraggingIterator->position += increment;
                gradient->DraggingIterator->position = ImClamp(gradient->DraggingIterator->position, 0.0f, 1.0f);
                gradient->RefreshCache();
                modified = true;
            }
            
            float diffY = ImGui::GetIO().MousePos.y - barBottom;
            
            if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
            {
                gradient->RemoveMark(gradient->DraggingIterator);
                gradient->DraggingIterator = gradient->GetMarks().end();
                gradient->SelectedIterator = gradient->GetMarks().end();
                modified = true;
            }
        }
        
        if (gradient->SelectedIterator == gradient->GetMarks().end() && !gradient->GetMarks().empty())
            gradient->SelectedIterator = gradient->GetMarks().begin();
        
        if (gradient->SelectedIterator != gradient->GetMarks().end() && ImGui::ColorPicker4("", &gradient->SelectedIterator->color[0]))
        {
            modified = true;
            gradient->RefreshCache();
        }
        
        return modified;
    }

    bool GradientEditorNoChange(Gradient *gradient, Gradient::MarkIterator &changeItem, ChangeType &changeType, glm::vec4 &color, f32 &position)
    {
        changeType = ChangeType::None;

        if (!gradient)
            return false;

        bool modified = false;

        ImVec2 bar_pos = ImGui::GetCursorScreenPos();
        bar_pos.x += 10;
        float maxWidth = ImGui::GetContentRegionAvailWidth() - 20;
        float barBottom = bar_pos.y + GRADIENT_BAR_EDITOR_HEIGHT;

        ImGui::InvisibleButton("gradient_editor_bar", ImVec2(maxWidth, GRADIENT_BAR_EDITOR_HEIGHT));

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(0))
        {
            modified = true;
            position = (ImGui::GetIO().MousePos.x - bar_pos.x) / maxWidth;
            color = gradient->GetColorAt(position);
            changeType = ChangeType::Add;
        }

        DrawGradientBar(gradient, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);
        DrawGradientMarks(gradient, gradient->DraggingIterator, gradient->SelectedIterator, bar_pos, maxWidth, GRADIENT_BAR_EDITOR_HEIGHT);

        if (!ImGui::IsMouseDown(0) && gradient->DraggingIterator != gradient->GetMarks().end())
            gradient->DraggingIterator = gradient->GetMarks().end();

        if (ImGui::IsMouseDragging(0) && gradient->DraggingIterator != gradient->GetMarks().end())
        {
            float increment = ImGui::GetIO().MouseDelta.x / maxWidth;
            bool insideZone = (ImGui::GetIO().MousePos.x > bar_pos.x) &&
                              (ImGui::GetIO().MousePos.x < bar_pos.x + maxWidth);

            if (increment != 0.0f && insideZone)
            {
                modified = true;
                position = ImClamp(gradient->DraggingIterator->position + increment, 0.0f, 1.0f);
                changeType = ChangeType::Position;
                changeItem = gradient->DraggingIterator;
            }

            float diffY = ImGui::GetIO().MousePos.y - barBottom;

            if (diffY >= GRADIENT_MARK_DELETE_DIFFY)
            {
                modified = true;
                changeType = ChangeType::Delete;
                changeItem = gradient->DraggingIterator;
            }
        }

        if (gradient->SelectedIterator == gradient->GetMarks().end() && !gradient->GetMarks().empty())
            gradient->SelectedIterator = gradient->GetMarks().begin();

        if (gradient->SelectedIterator != gradient->GetMarks().end())
        {
            glm::vec4 copyColor = gradient->SelectedIterator->color;

            if (ImGui::ColorPicker4("", &copyColor[0]))
            {
                modified = true;
                color = copyColor;
                changeType = ChangeType::Color;
                changeItem = gradient->SelectedIterator;
            }
        }

        return modified;
    }

    void GradientChange(Gradient *gradient, Gradient::MarkIterator &changeItem, ChangeType &changeType, glm::vec4 &color, f32 &position)
    {
        switch (changeType)
        {
        case ChangeType::Color:
            changeItem->color = color;
            gradient->RefreshCache();
            break;

        case ChangeType::Position:
            changeItem->position = position;
            gradient->RefreshCache();
            break;

        case ChangeType::Delete:
            gradient->RemoveMark(changeItem);
            gradient->DraggingIterator = gradient->GetMarks().end();
            gradient->SelectedIterator = gradient->GetMarks().end();
            gradient->RefreshCache();
            break;

        case ChangeType::Add:
            gradient->AddMark(position, color);
            gradient->RefreshCache();
            break;
        }
    }
};
