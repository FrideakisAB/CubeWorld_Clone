#ifndef ECSSMM_H
#define ECSSMM_H

#include <vector>
#include "ECS/API.h"

namespace Memory {
    class MemoryManager;

    extern MemoryManager *sMemoryManager;

    void InitializeMemoryManager();
    void TerminateMemoryManager();
}

namespace Memory {
    class MemoryManager {
    private:
        std::vector<std::pair<const char *, void *>> m_PendingMemory;

    public:
        MemoryManager();
        MemoryManager(const MemoryManager &) = delete;
        MemoryManager &operator=(MemoryManager &) = delete;

        void *Allocate(size_t memSize, const char *user = nullptr);
        void Free(void *pMem);
        void CheckMemoryLeaks();
    };
}

#endif