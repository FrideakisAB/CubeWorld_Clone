#include "Memory/Allocator/LinearAllocator.h"

namespace Memory::Allocator {
    LinearAllocator::LinearAllocator(size_t memSize, const void *mem) :
            IAllocator(memSize, mem) {}

    LinearAllocator::~LinearAllocator()
    {
        this->memoryUsed = 0;
        this->memoryAllocations = 0;
    }

    void *LinearAllocator::allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "Allocate called with memSize = 0");

        union {
            void *asVoidPtr;
            uptr asUptr;
        };

        asVoidPtr = (void *)this->memoryFirstAddress;
        asUptr += this->memoryUsed;

        u8 adjustment = GetAdjustment(asVoidPtr, alignment);

        if (this->memoryUsed + size + adjustment > this->memorySize)
        {
            return nullptr;
        }

        asUptr += adjustment;

        this->memoryUsed += size + adjustment;
        ++this->memoryAllocations;

        return asVoidPtr;
    }

    void LinearAllocator::free(void *p)
    {
        assert(false && "Linear allocators do not support free operations. Use clear instead.");
    }

    void LinearAllocator::clear()
    {
        this->memoryUsed = 0;
        this->memoryAllocations = 0;
    }
}