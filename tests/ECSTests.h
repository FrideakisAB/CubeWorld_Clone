#include <catch2/catch.hpp>

#include "ECS/ECS.h"
#include "Components/Transform.h"
#include "Memory/MemoryManager.h"

TEST_CASE("Test work Entity subsystem", "[Entity]")
{
    logger = new Log();
    Memory::InitializeMemoryManager();
    ECS::Initialize();

    struct manyEntity : ECS::Entity<manyEntity> {
        manyEntity() {} // Конструктор по умолчанию затирает поля IEntity
    };

    auto &EM = *ECS::ECS_Engine->GetEntityManager();

    SECTION("Test frequently used functions")
    {
        auto *ent = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        REQUIRE(ent->GetStaticEntityTypeID() == 0);
        REQUIRE(ent->IsActive() == true);
        REQUIRE(manyEntity::STATIC_ENTITY_TYPE_ID == 0);

        ent->SetActive(false);
        REQUIRE(ent->IsActive() == false);
        REQUIRE(ent->GetChildCount() == 0);
        REQUIRE(ent->GetEntityID() != ECS::INVALID_ENTITY_ID);
        REQUIRE(ent->GetParentID() == ECS::INVALID_ENTITY_ID);
        REQUIRE(ent->GetParent() == nullptr);

        auto *ent2 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());

        ent2->SetParent(ent);
        REQUIRE(ent2->GetParentID() == ent->GetEntityID());
        REQUIRE(ent2->IsActive() == false);

        ent->SetActive(true);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent->GetChildCount() == 1);
        REQUIRE(ent->GetChildID(0) == ent2->GetEntityID());
        REQUIRE(ent->GetChild(0) == ent2);

        ent2->SetParent(ECS::INVALID_ENTITY_ID);
        REQUIRE(ent2->GetParentID() == ECS::INVALID_ENTITY_ID);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent->GetChildCount() == 0);

        ent2->SetParent(ent->GetEntityID());
        REQUIRE(ent2->GetParentID() == ent->GetEntityID());

        ent2->SetParent(ECS::INVALID_ENTITY_ID);
        ent->AddChild(ent2);
        REQUIRE(ent2->GetParentID() == ent->GetEntityID());

        ent->RemoveChild(ent2->GetEntityID());
        ent->ReserveChildSpace(1);
        ent->AddChild(ent2->GetEntityID());
        REQUIRE(ent2->GetParentID() == ent->GetEntityID());

        ent->AddComponent<Transform>();
        REQUIRE(ent->GetComponent<Transform>() != nullptr);

        ent->RemoveComponent<Transform>();
        REQUIRE(ent->GetComponent<Transform>() == nullptr);

        delete ent;
        delete ent2;

        EM.DestroyEntity(ent->GetEntityID());
    }

    SECTION("Test of branch deletion behavior")
    {
        auto *ent = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        auto *ent2 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        auto *ent3 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        ent2->SetParent(ent);

        EM.DestroyEntity(ent->GetEntityID());
        ent3->AddChild(ent);
        ent3->AddChild(ent2);

        REQUIRE(ent3->GetChildCount() == 0);

        EM.DestroyEntity(ent3->GetEntityID());
    }

    SECTION("Test of active/inactive state behavior")
    {
        auto *ent = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        auto *ent2 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        auto *ent3 = (manyEntity*)EM.GetEntity(EM.CreateEntity<manyEntity>());
        ent2->SetParent(ent);
        ent2->AddChild(ent3);

        REQUIRE(ent->IsActive() == true);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent3->IsActive() == true);

        ent->SetActive(false);
        REQUIRE(ent->IsActive() == false);
        REQUIRE(ent2->IsActive() == false);
        REQUIRE(ent3->IsActive() == false);

        ent3->SetActive(false);
        ent->SetActive(true);
        REQUIRE(ent->IsActive() == true);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent3->IsActive() == false);

        ent2->SetActive(false);
        ent3->SetActive(true);
        REQUIRE(ent->IsActive() == true);
        REQUIRE(ent2->IsActive() == false);
        REQUIRE(ent3->IsActive() == false);

        ent2->SetActive(true);
        REQUIRE(ent->IsActive() == true);
        REQUIRE(ent2->IsActive() == true);
        REQUIRE(ent3->IsActive() == true);

        EM.DestroyEntity(ent->GetEntityID());
    }

    ECS::Terminate();
    Memory::TerminateMemoryManager();
    delete logger;
}
