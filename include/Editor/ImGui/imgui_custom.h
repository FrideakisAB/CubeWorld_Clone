#ifndef IMGUI_CUSTOM_H
#define IMGUI_CUSTOM_H

#include <string>
#include "imgui.h"
#include "Engine.h"
#include "Platform.h"
#include <glm/glm.hpp>
#include "Assets/AssetsManager.h"

enum class CustomTextState : u8 {
    None = 0,
    NoGlobal,
    Global,
    Invalid
};

namespace ImGui
{
    bool TextHandleButton(const std::string &label, const std::string &context, const std::string &type, CustomTextState state, u32 height);
    template<typename CMP>
    bool AssetSelectorPopup(const std::string &id, const std::string &name, const std::string &type, CustomTextState state, std::string &select, CMP cmp)
    {
        bool modify = false;

        if (ImGui::BeginPopup(id.c_str()))
        {
            bool isHave = false;
            for (const auto &value : GameEngine->GetAssetsManager())
            {
                if (cmp(std::cref(value.second)))
                {
                    isHave = true;
                    if (state != CustomTextState::Global || name != value.first)
                        ImGui::Text(value.first.c_str());
                    else
                        ImGui::TextColored(ImVec4(255, 230, 0,255), value.first.c_str());

                    if (ImGui::IsItemClicked())
                    {
                        if (state == CustomTextState::None || name != value.first)
                        {
                            modify = true;
                            select = value.first;
                        }

                        ImGui::CloseCurrentPopup();
                    }

                    ImGui::SameLine();
                    ImGui::TextColored(ImVec4(0,0,190,255), "(%s)", type.c_str());
                }
            }

            if (state != CustomTextState::None)
                ImGui::Text("None");
            else
                ImGui::TextColored(ImVec4(255, 230, 0,255), "None");

            if (ImGui::IsItemClicked())
            {
                if (state != CustomTextState::None)
                {
                    modify = true;
                    select = "";
                }

                ImGui::CloseCurrentPopup();
            }

            ImGui::SameLine();
            ImGui::TextColored(ImVec4(0,0,190,255), "(Void)");

            if (!isHave)
                ImGui::Text("Not have suitable assets");

            ImGui::EndPopup();
        }

        return modify;
    }
};

#endif
