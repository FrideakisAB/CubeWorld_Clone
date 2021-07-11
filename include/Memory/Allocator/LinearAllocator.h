#ifndef LINEARALLOC_H
#define LINEARALLOC_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class LinearAllocator final : public IAllocator {
    public:
        LinearAllocator(size_t memSize, const void *mem);
        ~LinearAllocator() final;

        void *allocate(size_t size, u8 alignment) override;
        void free(void *p) override;
        void clear() override;
    };
}

#endif