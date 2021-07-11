#include <catch2/catch.hpp>

#include "ECS/ECS.h"
#include "Memory/MemoryManager.h"

TEST_CASE("Test work Entity subsystem", "[Entity]")
{
    logger = new Log();
    Memory::InitializeMemory();
    ECS::Initialize();

    SECTION("Test often used functions")
    {
        struct manyEntity : ECS::Entity<manyEntity> {
            manyEntity() { } // Конструктор по умолчанию затирает поля IEntity
        };

        auto &EM = *ECS::ECS_Engine->GetEntityManager();

        auto *ent = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());

        REQUIRE(ent->IsActive() == true);

        ent->SetActive(false);
        REQUIRE(ent->IsActive() == false);

        REQUIRE(ent->GetChildCount() == 0);
        REQUIRE(ent->GetParentID() == ECS::INVALID_ENTITY_ID);

        auto *ent2 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());

        ent2->SetParent(ent);

        REQUIRE(ent2->GetParentID() == ent->GetEntityID());
        REQUIRE(ent2->IsActive() == false);

        ent->SetActive(true);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent->GetChildCount() == 1);
        REQUIRE(ent->GetChildID(0) == ent2->GetEntityID());

        ent2->SetParent(ECS::INVALID_ENTITY_ID);

        REQUIRE(ent2->GetParentID() == ECS::INVALID_ENTITY_ID);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent->GetChildCount() == 0);

        ent2->SetParent(ent);

        EM.DestroyEntity(ent->GetEntityID());
    }

    ECS::Terminate();
    Memory::TerminateMemory();
    delete logger;
}
