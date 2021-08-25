#ifndef IMENUENTRY_H
#define IMENUENTRY_H

#include <string>

class IMenuEntry {
private:
    std::string name;

public:
    explicit IMenuEntry(const std::string &name) : name(name) {}
    virtual ~IMenuEntry() = default;

    [[nodiscard]] const std::string &GetName() const noexcept { return name; }

    static bool RegisterItem(const std::string &name, const std::string &shortCuts = "", bool selected = false, bool enabled = true);
    static bool RegisterItemSelectable(const std::string &name, const std::string &shortCuts = "", bool *selected = nullptr, bool enabled = true);
    static bool RegisterSubMenu(const std::string &name, bool enabled = true);
    static void EndSubMenu();

    virtual void Draw() = 0;
};

#endif
