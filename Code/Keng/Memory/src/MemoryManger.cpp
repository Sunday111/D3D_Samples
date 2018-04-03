#include "MemoryManager.h"

namespace keng::memory
{
    void* MemoryManager::Allocate(size_t size) {
        //auto it = m_stats.find(size);
        //if (it == m_stats.end()) {
        //    m_stats[size] = 1;
        //} else {
        //    it->second += 1;
        //}

        void* result = nullptr;

             if (size <=   8) result =  m8.Allocate();
        else if (size <=  16) result = m16.Allocate();
        else if (size <=  32) result = m32.Allocate();
        else if (size <=  64) result = m64.Allocate();

        if (!result) {
            result = malloc(size);
        }

        assert(result);
        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        if (m8.TryDeallocate(pointer)) return;
        if (m16.TryDeallocate(pointer)) return;
        if (m32.TryDeallocate(pointer)) return;
        if (m64.TryDeallocate(pointer)) return;

        assert(pointer);
        free(pointer);
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
