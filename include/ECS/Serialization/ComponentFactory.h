#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "ECS/IEntity.h"
#include "ECS/ComponentManager.h"

namespace ECS {
    class ComponentFactory {
        class IComponentSet {
        public:
            virtual IComponent *Add(IEntity *ent) = 0;
            virtual ~IComponentSet() = default;
        };

        template<class T>
        class ComponentSet : public IComponentSet {
        public:
            IComponent *Add(IEntity *ent) override
            {
                return ent->AddComponent<T>(ent);
            }
        };

        using CmpSetRegistry = std::unordered_map<std::string, IComponentSet *>;
        CmpSetRegistry m_CmpSetRegistry;

        template<class T>
        inline ComponentSet<T> *GetComponentSet()
        {
            std::string CSID = typeid(T).name();

            auto it = this->m_CmpSetRegistry.find(CSID);
            ComponentSet<T> *ec = nullptr;

            if (it == this->m_CmpSetRegistry.end())
            {
                logger->Error("Component not regist!");
            }
            else
                ec = (ComponentSet<T> *)it->second;

            assert(ec != nullptr && "Failed to find ComponentSet<T>! Maybe component not register");
            return ec;
        }

    public:
        ~ComponentFactory()
        {
            for (auto cr : this->m_CmpSetRegistry)
            {
                delete cr.second;
                cr.second = nullptr;
            }
        }

        template<class T>
        std::string regist()
        {
            std::string CSID = typeid(T).name();

            auto it = this->m_CmpSetRegistry.find(CSID);

            if (it == this->m_CmpSetRegistry.end())
            {
                auto ec = new ComponentSet<T>();
                this->m_CmpSetRegistry[CSID] = ec;
                return CSID;
            }
            else
            {
                logger->Warning("Component has already register!");
                return CSID;
            }
        }

        IComponent *Add(const std::string &name, IEntity *ent)
        {
            return this->m_CmpSetRegistry[name]->Add(ent);
        }
    };
}

#endif