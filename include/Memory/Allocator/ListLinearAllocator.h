#ifndef LISTLINEARALLOC_H
#define LISTLINEARALLOC_H

#include <list>
#include "LinearAllocator.h"
#include "Memory/GlobalMemoryUser.h"

namespace Memory::Allocator {
    class ListLinearAllocator final : public IAllocator, private ::Memory::GlobalMemoryUser {
        using AllocatorsList = std::list<LinearAllocator *>;

    private:
        AllocatorsList allocators;
        AllocatorsList::iterator lastUsed;
        mutable u32 triggerAllocatorsCnt = 0;
        u32 usedAllocators = 0;
        size_t allocatorMemSize;
        const char *user;

    public:
        explicit ListLinearAllocator(size_t allocatorMemSize, const char *user = nullptr, u32 startCnt = 2);
        ~ListLinearAllocator() final;

        void *allocate(size_t size, u8 alignment) override;
        void free(void *p) override;
        void clear() override;
        void SetResizePolicy(u32 triggerAllocsCnt) const noexcept;
    };
}

#endif