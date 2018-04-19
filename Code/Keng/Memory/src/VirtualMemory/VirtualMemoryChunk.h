#pragma once

namespace keng::memory
{
    class VirtualMemoryChunk
    {
    public:
        VirtualMemoryChunk(size_t bytes, size_t pageSize);
        VirtualMemoryChunk(const VirtualMemoryChunk&) = delete;
        ~VirtualMemoryChunk();

        void Commit(size_t bytes);
        void* GetData() const;

    protected:
        size_t BytesToPages(size_t bytes) const;

    private:
        size_t m_pageSize = 100;
        size_t m_usedPages = 0;
        size_t m_reservedPages = 0;
        void* m_data = nullptr;
    };
}
