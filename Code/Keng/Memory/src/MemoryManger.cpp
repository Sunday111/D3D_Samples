#include "MemoryManager.h"

namespace keng::memory
{
    MemoryManager::MemoryManager() {
        m_log.open("MemoryManager.log", std::ios_base::out | std::ios_base::trunc);
    }

    void* MemoryManager::Allocate(size_t size) {
        //auto it = m_stats.find(size);
        //if (it == m_stats.end()) {
        //    m_stats[size] = 1;
        //} else {
        //    it->second += 1;
        //}

        auto tryAlloc = [&](auto& pool) -> void* {
            return pool.Allocate();
            //auto ptr = pool.Allocate();
            //if (ptr) {
            //    return ptr;
            //}
            //m_log << "Memory pool for " << pool.GetElementSize() << " bytes is full" << std::endl;
            //return nullptr;
        };

        void* result = nullptr;

             if (size <=   8) result = tryAlloc( m8);
        else if (size <=  16) result = tryAlloc(m16);
        else if (size <=  32) result = tryAlloc(m32);
        else if (size <=  64) result = tryAlloc(m64);

        if (!result) {
            result = malloc(size);
        }

        assert(result);
        return result;
    }

    void MemoryManager::Deallocate(void* pointer) {
        assert(pointer != nullptr);

        auto ok =
            m8.TryDeallocate(pointer) ||
            m16.TryDeallocate(pointer) ||
            m32.TryDeallocate(pointer) ||
            m64.TryDeallocate(pointer);

        if (!ok) {
            free(pointer);
        }
    }

    MemoryManager& MemoryManager::Instance() {
        static MemoryManager instance;
        return instance;
    }
}
