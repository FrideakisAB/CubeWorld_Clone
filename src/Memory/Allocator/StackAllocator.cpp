#include "Memory/Allocator/StackAllocator.h"

namespace Memory::Allocator {
    StackAllocator::StackAllocator(size_t memSize, const void *mem) :
            IAllocator(memSize, mem) {}

    StackAllocator::~StackAllocator()
    {
        this->clear();
    }

    void *StackAllocator::allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "allocate called with memSize = 0.");

        union {
            void *asVoidPtr;
            uptr asUptr;
            AllocMetaInfo *asMeta;
        };

        asVoidPtr = (void *)this->memoryFirstAddress;

        asUptr += this->memoryUsed;

        u8 adjustment = GetAdjustment(asVoidPtr, alignment, sizeof(AllocMetaInfo));

        if (this->memoryUsed + size + adjustment > this->memorySize)
            return nullptr;

        asMeta->adjustment = adjustment;

        asUptr += adjustment;

        this->memoryUsed += size + adjustment;
        this->memoryAllocations++;

        return asVoidPtr;
    }

    void StackAllocator::free(void *p)
    {
        union {
            void *asVoidPtr;
            uptr asUptr;
            AllocMetaInfo *asMeta;
        };

        asVoidPtr = p;

        asUptr -= sizeof(AllocMetaInfo);

        this->memoryAllocations--;
    }

    void StackAllocator::clear()
    {
        this->memoryUsed = 0;
        this->memoryAllocations = 0;
    }
}