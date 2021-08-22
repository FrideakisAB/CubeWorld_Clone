#ifndef MENUBAR_H
#define MENUBAR_H

#include <vector>

class IMenuEntry;

class MenuBar {
private:
    std::vector<IMenuEntry*> menuEntries;

public:
    void Draw();
    void RegisterEntry(IMenuEntry* menu);
};

#endif
