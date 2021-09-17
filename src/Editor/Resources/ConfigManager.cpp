#include "Editor/Resources/ConfigManager.h"

#include "imgui.h"
#include "Platform.h"

ConfigManager::ConfigManager()
{
    configs = json_utils::TryParse(Utils::FileToString(std::ifstream("configs.json")));
}

ConfigManager::~ConfigManager()
{
    std::ofstream file = std::ofstream("configs.json");
    std::string configsStr = configs.dump(4);
    file.write(configsStr.c_str(), configsStr.size());
}

void ConfigManager::SetConfig(const std::string &name, json config, bool isSettings)
{
    configs[name] = {config, isSettings};
}

json ConfigManager::GetConfig(const std::string &name) const
{
    if (auto It = configs.find(name); It != configs.end())
        return It.value()[0];

    return {};
}

void ConfigManager::DrawSettings()
{
    if (isOpen)
    {
        ImGui::OpenPopup("Settings");
        isOpen = false;
    }

    ImVec2 displaySize = ImGui::GetIO().DisplaySize;

    ImGui::SetNextWindowSize(ImVec2(displaySize.x * 0.7f, displaySize.y * 0.8f));
    displaySize.x /= 2.0f;
    displaySize.y /= 2.0f;
    ImGui::SetNextWindowPos(displaySize, 0, ImVec2(0.5f, 0.5f));
    bool isOpenModal = true;
    if (ImGui::BeginPopupModal("Settings", &isOpenModal, ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize))
    {
        isUpdate = false;
        for (auto &value: configs.items())
        {
            if (value.value()[1].get<bool>())
            {
                if (ImGui::CollapsingHeader(value.key().c_str()))
                {
                    json &config = value.value()[0];
                    for (auto &subValue: config.items())
                    {
                        bool subUpdate = drawSubMenu(subValue.value(), subValue.key());
                        if (subUpdate)
                            isUpdate = true;
                    }
                }
            }
        }

        if (!isOpenModal)
            ImGui::CloseCurrentPopup();

        ImGui::EndPopup();
    }
}

bool ConfigManager::drawSubMenu(json &node, const std::string &name)
{
    bool update = false;
    if (!node.is_object())
    {
        if (node.is_boolean())
        {
            bool local = node;
            update = ImGui::Checkbox(name.c_str(), &local);
            node = local;
        }
        else if (node.is_number_float())
        {
            f32 local = node;
            update = ImGui::InputFloat(name.c_str(), &local);
            node = local;
        }
        else if (node.is_number_integer())
        {
            i32 local = node;
            update = ImGui::InputInt(name.c_str(), &local);
            node = local;
        }
        else if (node.is_number_unsigned())
        {
            u32 local = node;
            update = ImGui::InputScalar(name.c_str(), ImGuiDataType_U32, &local);
            node = local;
        }
        else if (node.is_string())
        {
            std::string local = node;
            char localData[256];
            if (local.size() < 256)
                std::copy(local.begin(), local.end(), localData);
            update = ImGui::InputText(name.c_str(), localData, IM_ARRAYSIZE(localData));
            if (update)
                node = std::string(localData);
        }
    }
    else
    {
        if (name.find("color") != std::string::npos)
        {
            if (node.size() > 2 && node.size() < 5)
            {
                if (node.size() == 3)
                {
                    glm::vec3 color;
                    ns::from_json(node, color);
                    update = ImGui::ColorEdit3(name.c_str(), &color.x);
                    ns::to_json(node, color);
                }
                else if (node.size() == 4)
                {
                    glm::vec4 color;
                    ns::from_json(node, color);
                    update = ImGui::ColorEdit4(name.c_str(), &color.x);
                    ns::to_json(node, color);
                }
            }
        }
        else if (ImGui::TreeNode(name.c_str()))
        {
            for (auto &value: node.items())
            {
                update = drawSubMenu(value.value(), value.key());
                if (update)
                    isUpdate = true;
            }

            ImGui::TreePop();
        }
    }

    return update;
}
