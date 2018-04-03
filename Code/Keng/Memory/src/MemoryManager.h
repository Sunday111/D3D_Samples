#pragma once

#include <new>

namespace keng::memory
{
    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        MemoryManager() = default;
    };
}