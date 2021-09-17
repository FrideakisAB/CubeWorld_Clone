#ifndef CONFIGMANAGER_H
#define CONFIGMANAGER_H

#include <string>
#include "Utils/json.h"

class ConfigManager {
private:
    json configs = {};
    bool isUpdate = false;
    bool isOpen = false;

    bool drawSubMenu(json &node, const std::string &name);

public:
    ConfigManager();
    ~ConfigManager();

    void SetConfig(const std::string &name, json config, bool isSettings=false);
    [[nodiscard]] json GetConfig(const std::string &name) const;

    void DrawSettings();

    void OpenWindow() noexcept { isOpen = true; }
    [[nodiscard]] bool IsUpdate() const noexcept { return isUpdate; }
};

#endif
