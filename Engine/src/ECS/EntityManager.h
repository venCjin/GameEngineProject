#pragma once

#include "ComponentManager.h"

#include <bitset>
#include <vector>

//Maximal number of components per entity
const unsigned int MAX_COMPONENTS = 512;

template<typename T>
class ComponentHandle;
class EntityManager;
class EventManager;

//////////////////

class Entity
{
public:
    struct ID
    {
    private:
        uint64_t m_ID;
        bool m_Valid;

    public:
        ID() : m_ID(0), m_Valid(false) {}
        ID(uint64_t ID) : m_ID(ID), m_Valid(true) {}

        void Invalidate();
        uint64_t GetID() const { return m_ID; }

        operator bool() const { return m_Valid; }

        bool operator!=(const ID& other) const { return m_ID != other.m_ID; }
    };

private:
    EntityManager* m_EntityManager;
    ID m_ID;

public:
    Entity()
        : m_EntityManager(nullptr), m_ID() {}

    Entity(EntityManager* entityManager, Entity::ID id)
        : m_EntityManager(entityManager), m_ID(id) {}

    ID GetID() { return m_ID; }

    void Destroy();

    static bool Valid(ID id) { return id; }

    template <typename T, typename ... Args>
    ComponentHandle<T> AddComponent(Args&& ... args);

    template <typename T>
    ComponentHandle<T> AddComponentCopy(const T& component);

    template <typename T>
    void RemoveComponent();

    template <typename T>
    bool HasComponent();

    template <typename Component>
    ComponentHandle<Component> Component();

private:
    bool Valid() const { return m_ID; }
    void Invalidate();
};

//////////////////

class ComponentFamily
{
public:
    typedef uint64_t Family;

protected:
    static Family m_FamilyCounter;
};

template <typename T>
class ComponentF : public ComponentFamily
{
public:
    static Family GetFamily();
};

template <typename T>
ComponentFamily::Family ComponentF<T>::GetFamily()
{
    static Family family(m_FamilyCounter++);
    assert(family < MAX_COMPONENTS);
    return family;
}

//////////////////

template <typename T>
class ComponentHandle
{
    friend class EntityManager;

private:
    EntityManager* m_EntityManager;
    Entity::ID m_ID;

public:
    ComponentHandle() :
        m_EntityManager(nullptr) {}

    ComponentHandle(EntityManager* entityManager, Entity::ID id) :
        m_EntityManager(entityManager), m_ID(id) {}

    T* Get();
    T& operator*();
    T* operator->();
};

template<typename T>
T* ComponentHandle<T>::Get()
{
    return m_EntityManager->GetComponent<T>(m_ID);
}

template<typename T>
T& ComponentHandle<T>::operator*()
{
    return *m_EntityManager->GetComponent<T>(m_ID);
}

template<typename T>
T* ComponentHandle<T>::operator->()
{
    return m_EntityManager->GetComponent<T>(m_ID);
}

//////////////////

class EntityManager
{
    template <typename T>
    friend class ComponentHandle;

private:
    uint64_t m_IdCounter = 1;

    EventManager& m_EventManager;

    std::vector<std::bitset<MAX_COMPONENTS>> m_EntityComponentMask;
    std::vector<uint64_t> m_UnusedEntities;
    std::vector<uint64_t> m_EntityToDestroy;
    std::vector<PoolAllocator*> m_ComponentPools;

public:
    EntityManager(EventManager& eventManager)
        : m_EventManager(eventManager) {}

    Entity CreateEntity();
    void Destroy(Entity::ID entity);
    void DeferredDestroy();

    template <typename T, typename ... Args>
    ComponentHandle<T> AddComponent(Entity::ID id, Args&& ... args);

    template <typename T>
    void RemoveComponent(Entity::ID id);

    template <typename T>
    ComponentHandle<T> Component(Entity::ID id);

    template <typename ... Args>
    std::vector<Entity> EntitiesWithComponents();

    template <typename T>
    bool HasComponent(Entity::ID id) const;

private:
    template <typename T, typename ... Args>
    typename std::enable_if<sizeof...(Args) >= 1, bool>::type
        HasComponent(Entity::ID id) const;

    template <typename T>
    T* GetComponent(Entity::ID id);

    void AllocateEntity(uint64_t index);

    template <typename T>
    ComponentManager<T>* AllocateComponent();
};

template <typename T, typename ... Args>
ComponentHandle<T> EntityManager::AddComponent(Entity::ID id, Args&& ... args)
{
    assert(Entity::Valid(id));

    const ComponentFamily::Family family = ComponentF<T>::GetFamily();
    assert(!m_EntityComponentMask[id.GetID()].test(family));

    ComponentManager<T>* pool = AllocateComponent<T>();
    new(pool->Get(id.GetID())) T(std::forward<Args>(args) ...);

    m_EntityComponentMask[id.GetID()].set(family);

    ComponentHandle<T> component(this, id);
    return component;
}

template <typename T>
void EntityManager::RemoveComponent(Entity::ID id)
{
    assert(Entity::Valid(id));

    const ComponentFamily::Family family = ComponentF<T>::GetFamily();
    const uint64_t index = id.GetID();

    PoolAllocator* pool = m_ComponentPools[family];
    pool->Destroy(index);

    m_EntityComponentMask[id.GetID()].reset(family);

}

template <typename T>
ComponentHandle<T> EntityManager::Component(Entity::ID id)
{
    assert(Entity::Valid(id));

    if (HasComponent<T>(id))
        return ComponentHandle<T>(this, id);
    else
        return ComponentHandle<T>();
}

template <typename ... Args>
std::vector<Entity> EntityManager::EntitiesWithComponents()
{
    std::vector<Entity> result;

    for (uint64_t i = 1; i < m_EntityComponentMask.size(); i++)
    {
        Entity::ID id(i);
        if (HasComponent<Args ...>(Entity::ID(i)))
        {
            result.push_back(Entity(this, id));
        }
    }

    return result;
}

template <typename T>
bool EntityManager::HasComponent(Entity::ID id) const
{
    assert(Entity::Valid(id));

    const ComponentFamily::Family family = ComponentF<T>::GetFamily();

    if (family >= m_ComponentPools.size())
        return false;

    PoolAllocator* pool = m_ComponentPools[family];

    if (!pool || !m_EntityComponentMask[id.GetID()][family])
        return false;

    return true;
}

template <typename T, typename ... Args>
typename std::enable_if<sizeof...(Args) >= 1, bool>::type
EntityManager::HasComponent(Entity::ID id) const
{
    return HasComponent<T>(id) && HasComponent<Args ...>(id);
}

template <typename T>
T* EntityManager::GetComponent(Entity::ID id)
{
    assert(Entity::Valid(id));
    const ComponentFamily::Family family = ComponentF<T>::GetFamily();

    PoolAllocator* pool = m_ComponentPools[family];
    assert(pool);

    return static_cast<T*>(pool->Get(id.GetID()));
}

template <typename T>
ComponentManager<T>* EntityManager::AllocateComponent()
{
    const ComponentFamily::Family family = ComponentF<T>::GetFamily();

    if (m_ComponentPools.size() <= family)
    {
        m_ComponentPools.resize(family + 1);
    }

    if (!m_ComponentPools[family])
    {
        ComponentManager<T>* pool = new ComponentManager<T>();
        pool->AllocateEntity(m_IdCounter);
        m_ComponentPools[family] = pool;
    }

    return static_cast<ComponentManager<T>*>(m_ComponentPools[family]);
}

//////////////////

template <typename T, typename ... Args>
ComponentHandle<T> Entity::AddComponent(Args&& ... args)
{
    return m_EntityManager->AddComponent<T>(m_ID, std::forward<Args>(args) ...);
}

template<typename T>
inline ComponentHandle<T> Entity::AddComponentCopy(const T& component)
{
    return m_EntityManager->AddComponent<T>(m_ID, std::forward<const T&>(component));
}

template <typename T>
void Entity::RemoveComponent()
{
    m_EntityManager->RemoveComponent<T>(m_ID);
}

template <typename T>
bool Entity::HasComponent()
{
    return m_EntityManager->HasComponent<T>(m_ID);
}

template <typename Component>
ComponentHandle<Component> Entity::Component()
{
    return m_EntityManager->Component<Component>(m_ID);
}