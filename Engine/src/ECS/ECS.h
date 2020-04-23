#pragma once

#include <ECS/EventManager.h>
#include <ECS/EntityManager.h>
#include <ECS/SystemManager.h>

class EntityComponentSystem
{
protected:
    EventManager m_EventManager;
    EntityManager m_EntityManager;
    SystemManager m_SystemManager;

public:
    EntityComponentSystem()
        : m_EventManager(), m_EntityManager(m_EventManager), m_SystemManager(m_EntityManager, m_EventManager) {}

    EventManager* GetEventManager()   { return &m_EventManager; }
    EntityManager* GetEntityManager() { return &m_EntityManager; }
    SystemManager* GetSystemManager() { return &m_SystemManager; }
};