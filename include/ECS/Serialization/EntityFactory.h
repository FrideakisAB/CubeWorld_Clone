#ifndef ENTITYFACTORY_H
#define ENTITYFACTORY_H

#include "ECS/IEntity.h"
#include "ECS/EntityManager.h"
#include <boost/type_index.hpp>
#include "ECS/ComponentManager.h"

namespace ECS {
    class EntityFactory {
        class IEntitySet {
        public:
            virtual EntityId Create() = 0;
            virtual ~IEntitySet() = default;
        };

        template<class T>
        class EntitySet : public IEntitySet {
        public:
            EntityId Create() override
            {
                return ECS_Engine->GetEntityManager()->CreateEntity<T>();
            }
        };

        using EntSetRegistry = std::unordered_map<std::string, IEntitySet*>;
        EntSetRegistry entSetRegistry;

    public:
        ~EntityFactory()
        {
            for (auto es : entSetRegistry)
            {
                delete es.second;
                es.second = nullptr;
            }
        }

        template<class T>
        std::string Register()
        {
            std::string ESID = boost::typeindex::type_id<T>().pretty_name();

            if (entSetRegistry.find(ESID) == entSetRegistry.end())
                entSetRegistry[ESID] = new EntitySet<T>();

            return ESID;
        }

        EntityId Create(const std::string &name)
        {
            return entSetRegistry[name]->Create();
        }
    };
}

#endif
