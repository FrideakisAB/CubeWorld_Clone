#include "ECS/IComponent.h"

namespace ECS {
    IComponent::IComponent() :
            owner(INVALID_ENTITY_ID), enabled(true), treeLock(false) {}

    void IComponent::SetActive(bool active)
    {
        if (this->enabled == active)
            return;

        if (!active)
            this->OnDisable();
        else
            this->OnEnable();

        this->enabled = active;
    }
} 