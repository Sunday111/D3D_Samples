#pragma once

namespace keng::memory
{
    class VirtualMemoryChunk
    {
    public:
        VirtualMemoryChunk();
        VirtualMemoryChunk(size_t bytes, size_t pageSize);
        VirtualMemoryChunk(const VirtualMemoryChunk&) = delete;
        VirtualMemoryChunk(VirtualMemoryChunk&& that);
        ~VirtualMemoryChunk();

        void Commit(size_t bytes);
        void* GetData() const;

        VirtualMemoryChunk& operator=(const VirtualMemoryChunk&) = delete;
        VirtualMemoryChunk& operator=(VirtualMemoryChunk&& that);

    protected:
        size_t BytesToPages(size_t bytes) const;
        void MoveFrom(VirtualMemoryChunk& that);
        void Release();
        void Invalidate();
        bool IsValid() const;

    private:
        size_t m_pageSize = 100;
        size_t m_usedPages = 0;
        size_t m_reservedPages = 0;
        void* m_data = nullptr;
    };
}
