#include "pch.h"
#include "BufferLockManager.h"

BufferLockManager::BufferLockManager(bool cpuUpdates)
    : m_CPUUpdates(cpuUpdates)
{

}

BufferLockManager::~BufferLockManager()
{
    for (auto it = m_BufferLocks.begin(); it != m_BufferLocks.end(); it++) 
    {
        Cleanup(&*it);
    }

    m_BufferLocks.clear();
}

void BufferLockManager::WaitForLockedRange(size_t lockBeginBytes, size_t lockLength)
{
    BufferRange testRange = { lockBeginBytes, lockLength };
    std::vector<BufferLock> swapLocks;
    for (auto it = m_BufferLocks.begin(); it != m_BufferLocks.end(); ++it)
    {
        if (testRange.Overlaps(it->m_Range)) 
        {
            Wait(&it->m_SyncObj);
            Cleanup(&*it);
        }
        else 
        {
            swapLocks.push_back(*it);
        }
    }

    m_BufferLocks.swap(swapLocks);
}

void BufferLockManager::LockRange(size_t lockBeginBytes, size_t lockLength)
{
    GLsync sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
    BufferLock newLock = { { lockBeginBytes, lockLength }, sync };

    m_BufferLocks.push_back(newLock);
}

void BufferLockManager::Wait(GLsync* syncObj)
{
    if (m_CPUUpdates) 
    {
        GLbitfield waitFlags = 0;
        GLuint64 waitDuration = 0;
        while (true) 
        {
            GLenum waitRet = glClientWaitSync(*syncObj, waitFlags, waitDuration);
            if (waitRet == GL_ALREADY_SIGNALED || waitRet == GL_CONDITION_SATISFIED) 
            {
                return;
            }

            if (waitRet == GL_WAIT_FAILED) 
            {
                LOG_CORE_ERROR("Sync error");
                assert(true);
                return;
            }

            // After the first time, need to start flushing, and wait for a looong time.
            waitFlags = GL_SYNC_FLUSH_COMMANDS_BIT;
            waitDuration = 1000000000; // one second in nanoseconds
        }
    }
}

void BufferLockManager::Cleanup(BufferLock* bufferLock)
{
    glDeleteSync(bufferLock->m_SyncObj);
}
