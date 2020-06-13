#pragma once

#include <assert.h>

class PoolAllocator 
{
public:
    explicit PoolAllocator(size_t componentSize, size_t numberOfComponents)
        : m_ComponentSize(componentSize) 
    {
        m_Start = new char[componentSize * numberOfComponents];
    }

    virtual ~PoolAllocator()
    {
        delete[] m_Start;
    }

    size_t Size() const { return m_AllocatedEntities; }

    inline void AllocateEntity(size_t n) 
    {
        if (n > m_AllocatedEntities)
        {
            m_AllocatedEntities = n;
        }
    }

    inline void* Get(size_t n) 
    {
        assert(n < m_AllocatedEntities);
        uint64_t offset = n * m_ComponentSize;
        return m_Start + offset;
    }

    virtual void Destroy(size_t n) = 0;

protected:
    char* m_Start;
    size_t m_ComponentSize;
    size_t m_AllocatedEntities = 0;
};

template <typename T, size_t N = 5000>
class ComponentManager : public PoolAllocator 
{
public:
    ComponentManager() : PoolAllocator(sizeof(T), N) {}
    ~ComponentManager() {}

    void Destroy(size_t n) override 
    {
        assert(n < m_AllocatedEntities);
        T* obj = static_cast<T*>(Get(n));
        obj->~T();
    }
};


template <typename T, size_t N = 5000>
class FrameAllocator : public PoolAllocator
{
public:
    FrameAllocator() : PoolAllocator(sizeof(T), N) { m_AllocatedEntities = N; }
    ~FrameAllocator() {}

    void Destroy(size_t n) override
    {
        assert(n < m_AllocatedEntities);
        T* obj = static_cast<T*>(Get(n));
        obj->~T();
    }
};
