#include "Memory/GlobalMemoryUser.h"

#include "Memory/MemoryManager.h"

namespace Memory {
    GlobalMemoryUser::GlobalMemoryUser() : memoryManager(sMemoryManager) {}

    const void *GlobalMemoryUser::Allocate(size_t memSize, const char *user)
    {
        return memoryManager->Allocate(memSize, user);
    }

    void GlobalMemoryUser::Free(void *pMem)
    {
        memoryManager->Free(pMem);
    }
}