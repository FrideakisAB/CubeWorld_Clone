#ifndef GLOBALMEMORYUSER_H
#define GLOBALMEMORYUSER_H

#include <cstdlib>

namespace Memory {
    class MemoryManager;

    class GlobalMemoryUser {
    private:
        MemoryManager *memoryManager;

    public:
        GlobalMemoryUser();
        virtual ~GlobalMemoryUser() = default;

        const void *Allocate(size_t memSize, const char *user = nullptr);
        void Free(void *pMem);
    };
}

#endif
