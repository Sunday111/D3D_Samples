#pragma once

#include <new>
#include <map>
#include "Windows.h"
#include <cassert>
#include <vector>
#include <map>

namespace keng::memory
{
    template<size_t pageSize = 80>
    struct VirtualMemoryChunk
    {
        VirtualMemoryChunk(size_t bytes) {
            pagesCount = BytesToPages(bytes);
            data = VirtualAlloc(nullptr, pageSize * pagesCount, MEM_RESERVE, PAGE_NOACCESS);
        }

        ~VirtualMemoryChunk() {
            if (!data) {
                VirtualFree(data, 0, MEM_RELEASE);
                data = nullptr;
            }
            pagesCount = 0;
            commitedPages = 0;
        }

        void Commit(size_t bytes) {
            assert(bytes < pageSize * pagesCount);
            auto pages = BytesToPages(bytes);
            if (pages > commitedPages) {
                VirtualAlloc(data, pages * pageSize, MEM_COMMIT, PAGE_READWRITE);
                commitedPages = pages;
            }
        }

        static size_t BytesToPages(size_t bytes) {
            return (bytes / pageSize) + 1;
        }

        size_t pagesCount = 0;
        size_t commitedPages = 0;
        void* data = nullptr;
    };

    template<typename T, size_t resizeValue, size_t Capacity>
    class VirtualVector
    {
    public:
        VirtualVector() :
            m_memory(Capacity * sizeof(T))
        {}

        T& operator[](size_t index) {
            assert(index < m_size);
            return ((T*)m_memory.data)[index];
        }

        bool OwnsThisPointer(void* pointer) const {
            auto poolAddress = reinterpret_cast<size_t>(m_memory.data);
            auto pointerAddress = reinterpret_cast<size_t>(pointer);
            if (pointerAddress < poolAddress) {
                return false;
            }
            auto delta = pointerAddress - poolAddress;
            auto index = delta / sizeof(T);
            return index < m_size;
        }

        size_t GetSize() const { return m_size; }
        size_t GetStartAddress() const { return (size_t)m_memory.data; }

        void Resize(size_t size) {
            if (size > m_size) {
                m_memory.Commit(size * sizeof(T));
            }
            m_size = size;
        }

    private:
        size_t m_size = 0;
        VirtualMemoryChunk<resizeValue * sizeof(T)> m_memory;
    };

    template<size_t bytes>
    class AllocatedMemoryChunk
    {
    public:
        uint8_t data[bytes];
        bool busy;
    };

    template<size_t bytes, size_t resizeValue, size_t Capacity>
    class SmallChunkPool
    {
    public:
        using Chunk = AllocatedMemoryChunk<bytes>;
        using ChunkLayout = std::aligned_storage_t<sizeof(Chunk), sizeof(void*)>;

        void* Allocate() {
            auto size = m_vector.GetSize();
            for (size_t i = m_lastFree; i < Capacity && i < size; ++i) {
                if (!ChunckAt(i).busy) {
                    ChunckAt(i).busy = true;
                    m_lastFree = i;
                    return &(ChunckAt(i).data[0]);
                }
            }

            if (size == Capacity) {
                return nullptr;
            }

            m_lastFree = size;
            m_vector.Resize(size + 1);
            ChunckAt(size).busy = true;
            return &(ChunckAt(size).data[0]);
        }

        bool TryDeallocate(void* pointer) {
            if (!m_vector.OwnsThisPointer(pointer)) {
                return false;
            }

            auto a = m_vector.GetStartAddress();
            auto b = (size_t)pointer;
            auto i = (b - a) / sizeof(ChunkLayout);
            ChunckAt(i).busy = false;
            if (i < m_lastFree) {
                m_lastFree = i;
            }
            return true;
        }

    private:
        Chunk& ChunckAt(size_t index) {
            return (Chunk&)m_vector[index];
        }

        size_t m_lastFree = 0;
        VirtualVector<ChunkLayout, resizeValue, Capacity> m_vector;
    };

    class MemoryManager
    {
    public:
        void* Allocate(std::size_t size);
        void Deallocate(void* pointer);
        static MemoryManager& Instance();

    private:
        SmallChunkPool< 8, 10, 100000>  m8;
        SmallChunkPool<16, 10, 100000> m16;
        SmallChunkPool<32, 10, 100000> m32;
        SmallChunkPool<64, 10, 100000> m64;

        MemoryManager() = default;

        //std::map<size_t, size_t> m_stats;
    };
}