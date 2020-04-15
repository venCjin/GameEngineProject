#pragma once

#include <pch.h>
#include <ECS/EntityManager.h>

//////////////////

uint64_t ComponentFamily::m_FamilyCounter = 0;

//////////////////

const Entity::ID Entity::INVALID = 0;

//////////////////

void Entity::Invalidate()
{
    m_ID = INVALID;
    m_EntityManager = nullptr;
}

void Entity::Destroy()
{
    m_EntityManager->Destroy(m_ID);
    Invalidate();
}

//////////////////

Entity EntityManager::CreateEntity()
{
    uint64_t index;

    if (m_FreeList.empty()) {
        index = m_IdCounter++;
        AccomodateEntity(index);
    }
    else
    {
        index = m_FreeList.back();
        m_FreeList.pop_back();
    }

    Entity entity(this, Entity::ID(index));
    return entity;
}

void EntityManager::Destroy(Entity::ID id)
{
    assert(Entity::Valid(id));

    uint64_t index = id.GetID();
    auto mask = m_EntityComponentMask[index];
    for (size_t i = 0; i < m_ComponentPools.size(); i++)
    {
        BaseComponentTypeSaver* type = m_ComponentTypes[i];
        if (type && mask.test(i))
            type->RemoveComponent(Entity(this, id));
    }

    m_EntityComponentMask[index].reset();
    m_FreeList.push_back(index);
}

void EntityManager::AccomodateEntity(uint64_t index)
{
    if (m_EntityComponentMask.size() <= index)
    {
        m_EntityComponentMask.resize(index + 1);
        for (PoolAllocator* pool : m_ComponentPools)
            if (pool) pool->Expand(index + 1);
    }
}