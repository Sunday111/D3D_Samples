#pragma once

#include <new>
#include <memory>
#include "VirtualMemory/VirtualPool.h"

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

        std::vector<VirtualPool> m_pools;
        std::vector<VirtualPool*> m_sortedBySize;
        std::vector<VirtualPool*> m_sortedByStartAddress;
    };
}
