#pragma once

#include <algorithm>
#include "VirtualVector.h"
#include "EverydayTools/Bitset.h"

namespace keng::memory
{
    class VirtualPool
    {
    private:
        struct ChunkHeader
        {
            ChunkHeader* nextFree;
        };

    public:
        VirtualPool() {
            Invalidate();
        }

        VirtualPool(size_t elementSize, size_t resizeStep, size_t capacity) :
            m_elementSize(elementSize),
            m_chunkSize(ComputeElementSize(elementSize)),
            m_capacity(capacity),
            m_memory(m_capacity * m_chunkSize, resizeStep * m_chunkSize)
        {
        }

        VirtualPool(const VirtualPool&) = delete;

        VirtualPool(VirtualPool&& that) {
            MoveFrom(that);
        }

        ~VirtualPool() {
            Release();
        }

        void* Allocate() {
            assert(IsValid());

            if (m_nextFree == nullptr && (!Expand())) {
                return nullptr;
            }

            void* result = m_nextFree;
            m_nextFree = m_nextFree->nextFree;

            return result;
        }

        void Deallocate(void* pointer) {
            assert(IsValid());
            assert(HasThisPointer(pointer));
            auto p = (ChunkHeader*)pointer;
            p->nextFree = m_nextFree;
            m_nextFree = p;
        }

        size_t GetBeginAddress() const {
            assert(IsValid());
            return (size_t)m_memory.GetData();
        }

        size_t GetEndAddress() const {
            assert(IsValid());
            return m_capacity * m_chunkSize + GetBeginAddress();
        }

        size_t GetElementSize() const {
            return m_elementSize;
        }

        VirtualPool& operator=(VirtualPool&& that) {
            Release();
            MoveFrom(that);
            return *this;
        }

        VirtualPool& operator=(const VirtualPool&) = delete;

    protected:
        bool IsValid() const {
            return m_elementSize != 0;
        }

        void Invalidate() {
            m_elementSize = 0;
        }

        void MoveFrom(VirtualPool& that) {
            m_elementSize = that.m_elementSize;
            m_chunkSize = that.m_chunkSize;
            m_chunks = that.m_chunks;
            m_capacity = that.m_capacity;
            m_memory = std::move(that.m_memory);
            m_nextFree = that.m_nextFree;
            that.Invalidate();
        }

        void Release() {
            if (IsValid()) {
                CheckForFails();
                Invalidate();
            }
        }

        ChunkHeader* GetHeaderAt(size_t index) {
            assert(index < m_chunks);
            return (ChunkHeader*)(GetBeginAddress() + index * m_chunkSize);
        }

        bool Expand() {
            auto size = m_chunks;
            if (!Resize(size + 1)) {
                return false;
            }
            m_nextFree = GetHeaderAt(size);
            m_nextFree->nextFree = nullptr;
            return true;
        }

        static size_t ComputeElementSize(size_t allocSize) {
            return std::max(allocSize, sizeof(ChunkHeader));
        }

        void CheckForFails() {
            auto head = m_nextFree;
            edt::Bitset visited(m_chunks);

            while (head) {
                auto startAddress = GetBeginAddress();
                auto headAddress = (size_t)head;

                // Address does not belong to this pool
                assert(headAddress >= startAddress);

                // Invalid address
                assert(((headAddress - startAddress) % m_chunkSize) == 0);

                auto index = (headAddress - startAddress) / m_chunkSize;

                // Circle in free objects list check
                assert(!visited.IsSet(index));

                visited.Set(index);
                head = head->nextFree;
            }

            // Memory leak (or static object keeps reference...)
            assert(visited.All());
        }

        bool HasThisPointer(void* pointer) const {
            auto poolAddress = reinterpret_cast<size_t>(m_memory.GetData());
            auto pointerAddress = reinterpret_cast<size_t>(pointer);
            if (pointerAddress < poolAddress) {
                return false;
            }
            auto delta = (pointerAddress - poolAddress);
            auto index = delta / m_chunkSize;
            return index < m_chunks;
        }

        bool Resize(size_t size) {
            if (size > m_chunks) {
                if (size > m_capacity) {
                    return false;
                }
                m_memory.Commit(size * m_chunkSize);
            }
            m_chunks = size;
            return true;
        }

    private:
        size_t m_elementSize;
        size_t m_chunkSize;
        size_t m_chunks = 0;
        size_t m_capacity;
        VirtualMemoryChunk m_memory;
        ChunkHeader* m_nextFree = nullptr;
    };
}
