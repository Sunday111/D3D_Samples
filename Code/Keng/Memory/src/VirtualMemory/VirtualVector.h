#pragma once

#include "VirtualMemoryChunk.h"
#include <cassert>

namespace keng::memory
{
    template<typename T>
    class VirtualVector
    {
    public:
        VirtualVector(size_t capacity, size_t resizeValue = 1) :
            m_resizeValue(resizeValue),
            m_capacity(capacity),
            m_memory(m_capacity * sizeof(T), m_resizeValue * sizeof(T))
        {}

        T& operator[](size_t index) {
            assert(index < m_size);
            return ((T*)m_memory.GetData())[index];
        }

        bool OwnsThisPointer(void* pointer) const {
            auto poolAddress = reinterpret_cast<size_t>(m_memory.GetData());
            auto pointerAddress = reinterpret_cast<size_t>(pointer);
            if (pointerAddress < poolAddress) {
                return false;
            }
            auto delta = (pointerAddress - poolAddress);
            auto index = delta / sizeof(T);
            return index < m_size;
        }

        size_t GetSize() const { return m_size; }
        size_t GetStartAddress() const { return (size_t)m_memory.GetData(); }

        bool Resize(size_t size) {
            if (size > m_size) {
                if (size > m_capacity) {
                    return false;
                }
                m_memory.Commit(size * sizeof(T));
            }
            m_size = size;
            return true;
        }

    private:
        size_t m_size = 0;
        size_t m_resizeValue = 1;
        size_t m_capacity;
        VirtualMemoryChunk m_memory;
    };
}