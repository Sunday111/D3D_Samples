#include "Keng/Memory/Memory.h"
#include "MemoryManager.h"

namespace keng::memory
{
    void* Api::Allocate(std::size_t size) {
        return MemoryManager::Instance().Allocate(size);
    }

    void Api::Deallocate(void* pointer) {
        return MemoryManager::Instance().Deallocate(pointer);
    }
}
