#include "Memory/Allocator/IAllocator.h"

namespace Memory::Allocator {
    IAllocator::IAllocator(size_t memSize, const void *mem) :
            memorySize(memSize),
            memoryFirstAddress(mem),
            memoryUsed(0),
            memoryAllocations(0) {}
}