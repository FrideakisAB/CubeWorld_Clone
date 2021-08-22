#include "Editor/UI/MenuBar.h"

#include "imgui.h"
#include "Editor/UI/IMenuEntry.h"

void MenuBar::Draw()
{
    if (ImGui::BeginMainMenuBar())
    {
        for (auto &entry : this->menuEntries)
        {
            if(entry == nullptr) continue;
            if(ImGui::BeginMenu(entry->GetName().c_str()))
            {
                entry->Draw();
                ImGui::EndMenu();
            }
        }
        ImGui::EndMainMenuBar();
    }
}

void MenuBar::RegisterEntry(IMenuEntry *menu)
{
    if (menu != nullptr)
        menuEntries.push_back(menu);
}
