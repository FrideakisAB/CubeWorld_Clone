#ifndef MEMORYCHUNKALLOCATOR_H
#define MEMORYCHUNKALLOCATOR_H

#include <list>
#include "ECS/API.h"
#include "PoolAllocator.h"
#include "Memory/GlobalMemoryUser.h"

namespace Memory::Allocator {
    template<class OBJECT_TYPE, size_t MAX_CHUNK_OBJECTS>
    class MemoryChunkAllocator : protected ::Memory::GlobalMemoryUser {
    private:
        static const size_t MAX_OBJECTS = MAX_CHUNK_OBJECTS;
        static const size_t ALLOC_SIZE = (sizeof(OBJECT_TYPE) + alignof(OBJECT_TYPE)) * MAX_OBJECTS;
        const char *allocatorTag;

    public:
        using Allocator = Memory::Allocator::PoolAllocator;
        using ObjectList = std::list<OBJECT_TYPE *>;

        class MemoryChunk {
        public:
            Allocator *allocator;
            ObjectList objects;

            uptr chunkStart;
            uptr chunkEnd;

            explicit MemoryChunk(Allocator *allocator) :
                    allocator(allocator)
            {
                this->chunkStart = reinterpret_cast<uptr>(allocator->GetMemoryAddress0());
                this->chunkEnd = this->chunkStart + ALLOC_SIZE;
                this->objects.clear();
            }
        };

        using MemoryChunks = std::list<MemoryChunk *>;

        class iterator {
        public:
            using iterator_category = std::forward_iterator_tag;
            using value_type = OBJECT_TYPE;
            using difference_type = std::ptrdiff_t;
            using pointer = OBJECT_TYPE *;
            using reference = OBJECT_TYPE &;

        private:
            typename MemoryChunks::iterator currentChunk;
            typename MemoryChunks::iterator anEnd;
            typename ObjectList::iterator currentObject;

        public:
            iterator(typename MemoryChunks::iterator begin, typename MemoryChunks::iterator end) :
                    currentChunk(begin), anEnd(end)
            {
                if (begin != end)
                {
                    assert((*currentChunk) != nullptr);
                    currentObject = (*currentChunk)->objects.begin();

                    if (currentObject == (*currentChunk)->objects.end())
                        currentChunk = anEnd;
                }
                else
                {
                    currentObject = (*std::prev(anEnd))->objects.end();
                }
            }

            inline iterator &operator++()
            {
                // move to next object in current chunk
                ++currentObject;

                // if we reached end of list, move to next chunk
                if (currentObject == (*currentChunk)->objects.end())
                {
                    ++currentChunk;

                    if (currentChunk != anEnd)
                    {
                        // set object iterator to begin of next chunk list
                        assert((*currentChunk) != nullptr);
                        currentObject = (*currentChunk)->objects.begin();
                    }
                }

                return *this;
            }

            inline reference operator*() const { return *(*currentObject); }

            inline pointer operator->() const { return *currentObject; }

            inline bool operator==(const iterator &other) const
            {
                return ((this->currentChunk == other.currentChunk) && (this->currentObject == other.currentObject));
            }

            inline bool operator!=(const iterator &other) const
            {
                return (!((this->currentChunk == other.currentChunk) && (this->currentObject == other.currentObject)));
            }
        };

    protected:
        MemoryChunks chunks;

    public:
        explicit MemoryChunkAllocator(const char *allocatorTag = nullptr) :
                allocatorTag(allocatorTag)
        {
            auto *allocator = new Allocator(ALLOC_SIZE, Allocate(ALLOC_SIZE, allocatorTag), sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
            this->chunks.push_back(new MemoryChunk(allocator));
        }

        ~MemoryChunkAllocator() override
        {
            for (auto chunk : this->chunks)
            {
                for (auto obj : chunk->objects)
                    ((OBJECT_TYPE *) obj)->~OBJECT_TYPE();

                chunk->objects.clear();

                Free((void *)chunk->allocator->GetMemoryAddress0());
                delete chunk->allocator;
                chunk->allocator = nullptr;

                delete chunk;
                chunk = nullptr;
            }
        }

        void *CreateObject()
        {
            void *slot = nullptr;

            for (auto chunk : this->chunks)
            {
                if (chunk->objects.size() > MAX_OBJECTS)
                    continue;

                slot = chunk->allocator->allocate(sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
                if (slot != nullptr)
                {
                    chunk->objects.push_back((OBJECT_TYPE *)slot);
                    break;
                }
            }

            if (slot == nullptr)
            {
                auto *allocator = new Allocator(ALLOC_SIZE, Allocate(ALLOC_SIZE, this->allocatorTag), sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));
                auto *newChunk = new MemoryChunk(allocator);

                this->chunks.push_front(newChunk);

                slot = newChunk->allocator->allocate(sizeof(OBJECT_TYPE), alignof(OBJECT_TYPE));

                assert(slot != nullptr && "Unable to create new object. Out of memory?!");
                newChunk->objects.clear();
                newChunk->objects.push_back((OBJECT_TYPE *)slot);
            }

            return slot;
        }

        void DestroyObject(void *object)
        {
            uptr adr = reinterpret_cast<uptr>(object);

            auto chunk = std::find_if(this->chunks.begin(), this->chunks.end(), [adr](MemoryChunk *chunk) {
                return (chunk->chunkStart <= adr && adr < chunk->chunkEnd);
            });

            if (chunk == this->chunks.end())
            {
                logger->Error("Failed to delete object. Memory corruption!");
                return;
            }

            (*chunk)->objects.remove((OBJECT_TYPE *) object);
            (*chunk)->allocator->free(object);
        }

        inline iterator begin() { return iterator(this->chunks.begin(), this->chunks.end()); }

        inline iterator end() { return iterator(this->chunks.end(), this->chunks.end()); }
    };
}

#endif 