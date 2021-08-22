#include "Editor/UI/IMenuEntry.h"

#include "imgui.h"

bool IMenuEntry::RegisterItem(const std::string &name, const std::string &shortCuts, bool selected, bool enabled)
{
    return ImGui::MenuItem(name.c_str(), shortCuts.c_str(), selected, enabled);
}

bool IMenuEntry::RegisterSubMenu(const std::string &name, bool enabled)
{
    return ImGui::BeginMenu(name.c_str(), enabled);
}

void IMenuEntry::EndSubMenu()
{
    ImGui::EndMenu();
}
