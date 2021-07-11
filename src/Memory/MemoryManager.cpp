#include "Memory/MemoryManager.h"

#include <algorithm>

namespace Memory {
    MemoryManager *sMemoryManager;

    void InitializeMemory()
    {
        sMemoryManager = new MemoryManager();
    }

    void TerminateMemory()
    {
        sMemoryManager->CheckMemoryLeaks();
        delete sMemoryManager;
        sMemoryManager = nullptr;
    }
}

namespace Memory {
    MemoryManager::MemoryManager()
    {
        this->m_PendingMemory.clear();
    }

    void MemoryManager::CheckMemoryLeaks()
    {
        if (!this->m_PendingMemory.empty())
        {
            for (auto i : this->m_PendingMemory)
            {
                logger->Error("%s memory user didn't release allocated memory %llu", i.first, (u64) i.second);
                Free(i.second);
            }
        }
        else
            logger->Info("No memory leaks detected");
    }

    void *MemoryManager::Allocate(size_t memSize, const char *user)
    {
        void *pMemory = malloc(memSize);
        this->m_PendingMemory.emplace_back(user, pMemory);

        return pMemory;
    }

    void MemoryManager::Free(void *pMem)
    {
        auto it = std::find_if(this->m_PendingMemory.begin(), this->m_PendingMemory.end(), [&pMem](std::pair<const char *, void *> val) {
            return (val.second == pMem);
        });
        if (it != this->m_PendingMemory.end())
        {
            this->m_PendingMemory.erase(it);
            free(pMem);
        }
    }
}