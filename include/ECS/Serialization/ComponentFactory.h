#ifndef COMPONENTFACTORY_H
#define COMPONENTFACTORY_H

#include "ECS/IEntity.h"
#include <boost/type_index.hpp>
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
                return ent->AddComponent<T>();
            }
        };

        using CmpSetRegistry = std::unordered_map<std::string, IComponentSet *>;
        CmpSetRegistry cmpSetRegistry;

        template<class T>
        inline ComponentSet<T> *GetComponentSet()
        {
            std::string CSID = boost::typeindex::type_id<T>().pretty_name();

            auto it = this->cmpSetRegistry.find(CSID);
            ComponentSet<T> *ec = nullptr;

            if (it == this->cmpSetRegistry.end())
            {
                logger->Error("Component not register!");
            }
            else
                ec = (ComponentSet<T> *)it->second;

            assert(ec != nullptr && "Failed to find ComponentSet<T>! Maybe component not register");
            return ec;
        }

    public:
        ~ComponentFactory()
        {
            for (auto cr : this->cmpSetRegistry)
            {
                delete cr.second;
                cr.second = nullptr;
            }
        }

        template<class T>
        std::string Register()
        {
            std::string CSID = boost::typeindex::type_id<T>().pretty_name();

            if (this->cmpSetRegistry.find(CSID) == this->cmpSetRegistry.end())
                this->cmpSetRegistry[CSID] = new ComponentSet<T>();

            return CSID;
        }

        IComponent *Add(const std::string &name, IEntity *ent)
        {
            return this->cmpSetRegistry[name]->Add(ent);
        }
    };
}

#endif