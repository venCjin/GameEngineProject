#pragma once

#include <vector>

#include "glad/glad.h"

struct BufferRange
{
    size_t m_StartOffset;
    size_t m_Length;

    bool Overlaps(const BufferRange& other) const {
        return m_StartOffset < (other.m_StartOffset + other.m_Length)
            && other.m_StartOffset < (m_StartOffset + m_Length);
    }
};

struct BufferLock
{
    BufferRange m_Range;
    GLsync m_SyncObj;
};

class BufferLockManager
{
public:
    BufferLockManager(bool cpuUpdates);
    ~BufferLockManager();

    void WaitForLockedRange(size_t lockBeginBytes, size_t lockLength);
    void LockRange(size_t lockBeginBytes, size_t lockLength);

private:
    void Wait(GLsync* syncObj);
    void Cleanup(BufferLock* bufferLock);

    std::vector<BufferLock> m_BufferLocks;

    bool m_CPUUpdates;
};
