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

        VirtualPool< 8>  m8;
        VirtualPool<16> m16;
        VirtualPool<32> m32;
        VirtualPool<64> m64;
    };
}
