#pragma once

#include <new>
#include <memory>
#include "VirtualMemory/VirtualPool.h"
#include <vector>
#include <unordered_map>

namespace keng::memory
{
    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        MemoryManager();
        ~MemoryManager();

        void RegisterAllocation(size_t size);

        size_t m_biggestPoolSize = 0;
        std::vector<VirtualPool> m_pools;
        std::vector<VirtualPool*> m_sortedBySize;
        std::vector<VirtualPool*> m_sortedByStartAddress;
    };
}
