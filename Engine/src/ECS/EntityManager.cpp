#pragma once

#include <pch.h>
#include <ECS/EntityManager.h>

//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
#include <Gameplay/Components/Mesh.h>

#include <Physics/Components/BoxCollider.h>
#include <Physics/Components/SphereCollider.h>
//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

//////////////////

uint64_t ComponentFamily::m_FamilyCounter = 0;

//////////////////

void Entity::ID::Invalidate()
{
    m_Valid = false;
}

//////////////////

void Entity::Invalidate()
{
    m_ID.Invalidate();
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

    if (m_UnusedEntities.empty()) {
        index = m_IdCounter++;
        AllocateEntity(index);
    }
    else
    {
        index = m_UnusedEntities.back();
        m_UnusedEntities.pop_back();
    }

    return Entity(this, Entity::ID(index));
}

void EntityManager::Destroy(Entity::ID id)
{
    assert(Entity::Valid(id));

    uint64_t index = id.GetID();
    /*auto mask = m_EntityComponentMask[index];
    for (size_t i = 0; i < m_ComponentPools.size(); i++)
    {
        PoolAllocator* pool = m_ComponentPools[i];
        if (mask.test(i))
            pool->Destroy(index);
    }*/

    m_EntityToDestroy.push_back(index);
}

void EntityManager::DeferredDestroy()
{
    for (auto i : m_EntityToDestroy)
    {
        Entity entity(this, i);
        if (entity.HasComponent<sixengine::Mesh>())
            entity.RemoveComponent<sixengine::Mesh>();

        if (entity.HasComponent<sixengine::BoxCollider>())
            entity.RemoveComponent<sixengine::BoxCollider>();

        if (entity.HasComponent<sixengine::SphereCollider>())
            entity.RemoveComponent<sixengine::SphereCollider>();

        //m_EntityComponentMask[i].reset();
        //m_UnusedEntities.push_back(i);
    }

    m_EntityToDestroy.clear();
}

void EntityManager::AllocateEntity(uint64_t index)
{
    if (m_EntityComponentMask.size() <= index)
    {
        m_EntityComponentMask.resize(index + 1);
        for (PoolAllocator* pool : m_ComponentPools)
            if (pool) pool->AllocateEntity(index + 1);
    }
}