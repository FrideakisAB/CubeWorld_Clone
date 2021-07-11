#ifndef STACKALLOC_H
#define STACKALLOC_H

#include "IAllocator.h"

namespace Memory::Allocator {
    class StackAllocator final : public IAllocator {
        struct AllocMetaInfo {
            u8 adjustment;
        };

    public:
        StackAllocator(size_t memSize, const void *mem);
        ~StackAllocator() final;

        void *allocate(size_t size, u8 alignment) override;
        void free(void *p) override;
        void clear() override;
    };
}

#endif