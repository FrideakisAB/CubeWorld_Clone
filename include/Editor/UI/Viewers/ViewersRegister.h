#ifndef VIEWERSREGISTER_H
#define VIEWERSREGISTER_H

#include <map>
#include "ECS/ECS.h"
#include "Editor/UI/Viewers/IViewer.h"

class ViewersRegister {
private:
    std::map<ECS::ComponentTypeId, IViewer*> viewers;

public:
    ~ViewersRegister()
    {
        for (auto &&[id, viewer] : viewers)
            delete viewer;
    }

    template<class Vwr, class Cmp>
    void RegisterViewer()
    {
        if (viewers.find(Cmp::STATIC_COMPONENT_TYPE_ID) != viewers.end())
            delete viewers[Cmp::STATIC_COMPONENT_TYPE_ID];

        viewers[Cmp::STATIC_COMPONENT_TYPE_ID] = new Vwr();
    }

    [[nodiscard]] IViewer *GetViewer(ECS::ComponentTypeId id) const
    {
        if (auto It = viewers.find(id); It != viewers.end())
            return It->second;
        else
            return nullptr;
    }
};

#endif
