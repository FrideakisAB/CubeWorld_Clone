#include "Memory/Allocator/PoolAllocator.h"

namespace Memory::Allocator {
    PoolAllocator::PoolAllocator(size_t memSize, const void *mem, size_t objectSize, u8 objectAlignment) :
            IAllocator(memSize, mem),
            OBJECT_SIZE(objectSize),
            OBJECT_ALIGNMENT(objectAlignment)
    {
        this->clear();
    }

    PoolAllocator::~PoolAllocator()
    {
        this->freeList = nullptr;
    }

    void *PoolAllocator::allocate(size_t size, u8 alignment)
    {
        assert(size > 0 && "allocate called with memSize = 0.");
        assert(size == this->OBJECT_SIZE && alignment == this->OBJECT_ALIGNMENT);

        if (this->freeList == nullptr)
            return nullptr;

        // get free slot
        void *p = this->freeList;

        // point to next free slot
        this->freeList = (void **)(*this->freeList);

        this->memoryUsed += this->OBJECT_SIZE;
        this->memoryAllocations++;

        return p;
    }

    void PoolAllocator::free(void *mem)
    {
        *((void **)mem) = this->freeList;

        this->freeList = (void **)mem;

        this->memoryUsed -= this->OBJECT_SIZE;
        this->memoryAllocations--;
    }

    void PoolAllocator::clear()
    {
        u8 adjustment = GetAdjustment(this->memoryFirstAddress, this->OBJECT_ALIGNMENT);

        auto numObjects = (size_t)floor((this->memorySize - adjustment) / this->OBJECT_SIZE);

        union {
            void *asVoidPtr;
            uptr asUptr;
        };

        asVoidPtr = (void *)this->memoryFirstAddress;

        asUptr += adjustment;

        this->freeList = (void **)asVoidPtr;

        void **p = this->freeList;

        for (size_t i = 0; i < (numObjects - 1); ++i)
        {
            *p = (void *)((uptr)p + this->OBJECT_SIZE);

            p = (void **)*p;
        }

        *p = nullptr;

        this->memoryUsed = 0;
        this->memoryAllocations = 0;
    }
}