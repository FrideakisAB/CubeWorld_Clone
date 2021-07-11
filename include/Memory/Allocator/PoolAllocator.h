#ifndef POOLALLOC_H
#define POOLALLOC_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class PoolAllocator final : public IAllocator {
    private:
        const size_t OBJECT_SIZE;
        const u8 OBJECT_ALIGNMENT;
        void **freeList;

    public:
        PoolAllocator(size_t memSize, const void *mem, size_t objectSize, u8 objectAlignment);
        ~PoolAllocator() final;

        void *allocate(size_t size, u8 alignment) override;
        void free(void *mem) override;
        void clear() override;
    };
}

#endif