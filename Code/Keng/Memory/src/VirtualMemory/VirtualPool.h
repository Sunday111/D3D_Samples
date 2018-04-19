#pragma once

#include "VirtualVector.h"

#include <bitset>
#include <vector>

namespace keng::memory
{
    class Bitset
    {
    public:
        static constexpr auto partSizeBytes = sizeof(void*);
        static constexpr auto partSizeBits = partSizeBytes * 8;
        using Part = std::bitset<partSizeBits>;

        Bitset(size_t size) {
            m_parts.resize(size / partSizeBits + 1);
        }

        void Set(size_t index, bool value = true) {
            auto partIndex = index / partSizeBits;
            auto indexInPart = index % partSizeBits;
            m_parts[partIndex].set(indexInPart, value);
        }

        bool IsSet(size_t index) const {
            auto partIndex = index / partSizeBits;
            auto indexInPart = index % partSizeBits;
            return m_parts[partIndex].test(indexInPart);
        }

        std::vector<Part> m_parts;
    };

    template<size_t bytes>
    class VirtualPool
    {
    private:
        struct Chunk
        {
            static volatile size_t csz;

            struct Header
            {
                Chunk* nextFree;
            };

            struct Data
            {
                uint8_t mem[bytes];
            };

            union Body
            {
                Header header;
                Data data;
            };

            Body body;
        };

    public:
        VirtualPool(size_t resize, size_t capacity) :
            m_vector(capacity, resize)
        {
        }

        ~VirtualPool() {
            size_t freeCount = 0;
            auto head = m_nextFree;
            Bitset visited(m_vector.GetSize());
            while (head)
            {
                ++freeCount;
            
                auto startAddress = m_vector.GetStartAddress();
                auto thisAddress = (size_t)head;
                assert(thisAddress >= startAddress);
                assert(((thisAddress - startAddress) % bytes) == 0);
                auto index = (thisAddress - startAddress) / bytes;
                assert(!visited.IsSet(index));
                visited.Set(index);
                head = head->body.header.nextFree;
            }
            assert(freeCount == m_vector.GetSize());
        }

        void* Allocate() {
            if (m_nextFree == nullptr && (!Expand())) {
                return nullptr;
            }

            void* result = &(m_nextFree->body.data.mem[0]);
            m_nextFree = m_nextFree->body.header.nextFree;

            //if constexpr (bytes == 64) {
            //    if ((size_t)result == m_vector.GetStartAddress() + 31 * bytes) {
            //        assert(false);
            //    }
            //}

            return result;
        }

        bool TryDeallocate(void* pointer) {
            if (!m_vector.OwnsThisPointer(pointer)) {
                return false;
            }
            auto p = (Chunk*)pointer;
            p->body.header.nextFree = m_nextFree;
            m_nextFree = p;
            return true;
        }

        bool Expand() {
            auto size = m_vector.GetSize();
            if (!m_vector.Resize(size + 1)) {
                return false;
            }
            m_nextFree = (Chunk*)&m_vector[size];
            m_nextFree->body.header.nextFree = nullptr;
            return true;
        }

    private:
        Chunk& ChunckAt(size_t index) {
            return (Chunk&)m_vector[index];
        }

        VirtualVector<Chunk> m_vector;
        Chunk* m_nextFree = nullptr;
    };
}
