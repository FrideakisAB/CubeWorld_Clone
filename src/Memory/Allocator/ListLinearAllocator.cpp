#include "Memory/Allocator/ListLinearAllocator.h"

#include "ECS/API.h"

namespace Memory::Allocator {
    ListLinearAllocator::ListLinearAllocator(size_t allocatorMemSize, const char *user, u32 startCnt) :
            allocatorMemSize(allocatorMemSize), IAllocator(0, nullptr), user(user)
    {
        for (u32 i = 0; i < startCnt; ++i)
            allocators.push_back(new LinearAllocator(allocatorMemSize, Allocate(allocatorMemSize, user)));

        lastUsed = allocators.begin();
    }

    ListLinearAllocator::~ListLinearAllocator()
    {
        for (auto &&allocator : allocators)
        {
            Free((void *)allocator->GetMemoryAddress0());
            delete allocator;
            allocator = nullptr;
        }

        allocators.clear();
    }

    void *ListLinearAllocator::allocate(size_t size, u8 alignment)
    {
        void *mem = (*lastUsed)->allocate(size, alignment);

        if (mem != nullptr)
        {
            this->memoryUsed += size;
            ++this->memoryAllocations;
            return mem;
        }

        ++lastUsed;
        if (lastUsed == allocators.end())
        {
            allocators.push_back(new LinearAllocator(allocatorMemSize, Allocate(allocatorMemSize, user)));
            lastUsed = --allocators.end();
        }

        ++usedAllocators;

        mem = (*lastUsed)->allocate(size, alignment);

        this->memoryUsed += size;
        ++this->memoryAllocations;

        return mem;
    }

    void ListLinearAllocator::free(void *p)
    {
        assert(false && "Linear allocators do not support free operations. Use clear instead.");
    }

    void ListLinearAllocator::clear()
    {
        lastUsed = allocators.begin();
        for (u32 i = 0; i <= usedAllocators; ++i)
        {
            (*lastUsed)->clear();
            ++lastUsed;
        }

        lastUsed = allocators.begin();

        if (triggerAllocatorsCnt != 0 && (allocators.size() - (usedAllocators + 1)) >= triggerAllocatorsCnt)
        {
            for (u32 i = 0; i < triggerAllocatorsCnt; ++i)
            {
                auto It = --allocators.end();
                Free((void *)(*It)->GetMemoryAddress0());
                delete *It;
                *It = nullptr;
                allocators.erase(It);
            }
        }

        usedAllocators = 0;
        this->memoryUsed = 0;
        this->memoryAllocations = 0;
    }

    void ListLinearAllocator::SetResizePolicy(u32 triggerAllocsCnt) const noexcept
    {
        triggerAllocatorsCnt = triggerAllocsCnt;
    }
}
