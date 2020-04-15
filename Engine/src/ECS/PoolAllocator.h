
#include <assert.h>
#include <vector>

class PoolAllocator 
{
public:
    explicit PoolAllocator(std::size_t elementSize, std::size_t chunkSize = 8192)
        : m_ElementSize(elementSize), m_ChunkSize(chunkSize) {}

    virtual ~PoolAllocator()
    {
        for (char* ptr : m_Blocks)
            delete[] ptr;
    }

    std::size_t Size() const { return m_Size; }
    std::size_t Capacity() const { return m_Capacity; }
    std::size_t Chunks() const { return m_Blocks.size(); }

    inline void Expand(std::size_t n) 
    {
        if (n >= m_Size) 
        {
            if (n >= m_Capacity) 
                Reserve(n);

            m_Size = n;
        }
    }

    inline void Reserve(std::size_t n) 
    {
        while (m_Capacity < n) 
        {
            char* chunk = new char[m_ElementSize * m_ChunkSize];
            m_Blocks.push_back(chunk);
            m_Capacity += m_ChunkSize;
        }
    }

    inline void* Get(std::size_t n) 
    {
        assert(n < m_Size);
        uint64_t offset = (n % m_ChunkSize) * m_ElementSize;
        return m_Blocks[n / m_ChunkSize] + offset;
    }

    inline const void* Get(std::size_t n) const {
        assert(n < m_Size);
        uint64_t offset = (n % m_ChunkSize) * m_ElementSize;
        return m_Blocks[n / m_ChunkSize] + offset;
    }

    virtual void Destroy(std::size_t n) = 0;

protected:
    std::vector<char*> m_Blocks;
    std::size_t m_ElementSize;
    std::size_t m_ChunkSize;
    std::size_t m_Size = 0;
    std::size_t m_Capacity = 0;
};

template <typename T>
class ComponentPool : public PoolAllocator 
{
public:
    ComponentPool() : PoolAllocator(sizeof(T), 8192) {}
    ~ComponentPool() {}

    void Destroy(std::size_t n) override 
    {
        assert(n < m_Size);
        T* ptr = static_cast<T*>(Get(n));
        ptr->~T();
    }
};