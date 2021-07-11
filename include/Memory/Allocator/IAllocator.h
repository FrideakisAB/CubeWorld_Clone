#ifndef IALLOC_H
#define IALLOC_H

#include "ECS/API.h"

namespace Memory::Allocator {
	static inline void* AlignForward(void* address, u8 alignment)
	{
		return (void*)((reinterpret_cast<uptr>(address)+static_cast<uptr>(alignment - 1)) & static_cast<uptr>(~(alignment - 1)));
	}
	
	static inline u8 GetAdjustment(const void* address, u8 alignment)
	{
		u8 adjustment = alignment - (reinterpret_cast<uptr>(address)& static_cast<uptr>(alignment - 1));
	
		return adjustment == alignment ? 0 : adjustment;
	}
	
	static inline u8 GetAdjustment(const void* address, u8 alignment, u8 extra)
	{
		u8 adjustment = GetAdjustment(address, alignment);
		u8 neededSpace = extra;
	
		if(adjustment < neededSpace)
		{
			neededSpace -= adjustment;
	
			//Increase adjustment to fit header
			adjustment += alignment * (neededSpace / alignment);
	
			if(neededSpace % alignment > 0)
				adjustment += alignment;
		}
	
		return adjustment;
	}
	
	class IAllocator {
	protected:
		const size_t		memorySize;
		const void*			memoryFirstAddress;
	
		size_t				memoryUsed;
		u64					memoryAllocations;
	
	public:
		IAllocator(size_t memSize, const void* mem);
		virtual ~IAllocator() = default;
	
		virtual void* allocate(size_t size, u8 alignment) = 0;
		virtual void free(void* p) = 0;
		virtual void clear() = 0;

        [[nodiscard]] inline size_t GetMemorySize() const
		{
			return this->memorySize;
		}

        [[nodiscard]] inline const void* GetMemoryAddress0() const
		{
			return this->memoryFirstAddress;
		}

        [[nodiscard]] inline size_t GetUsedMemory() const
		{
			return this->memoryUsed;
		}
	
		[[nodiscard]] inline u64 GetAllocationCount() const
		{
			return this->memoryAllocations;
		}
	};
}

#endif 