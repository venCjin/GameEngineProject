#pragma once

#include "EventManager.h"
#include "PoolAllocator.h"

#include <bitset>
#include <vector>

//Maximal number of components per entity
const unsigned int MAX_COMPONENTS = 512;

template<typename T>
class ComponentHandle;
class EntityManager;

//////////////////

class Entity
{
public:
    struct ID
    {
    private:
        uint64_t m_ID;

    public:
        ID() : m_ID(0) {}
        ID(uint64_t ID) : m_ID(ID) {}

        uint64_t GetID() const { return m_ID; }

        bool operator==(const ID& other) const { return m_ID == other.m_ID; }
        bool operator!=(const ID& other) const { return m_ID != other.m_ID; }
    };

private:
    static const ID INVALID;

    EntityManager* m_EntityManager;
    ID m_ID;

public:
    Entity()
        : m_EntityManager(nullptr), m_ID() {}

    Entity(EntityManager* entityManager, Entity::ID id)
        : m_EntityManager(entityManager), m_ID(id) {}

    ID GetID() { return m_ID; }

    void Destroy();

    static bool Valid(ID id) { return id != INVALID; }

    template <typename T, typename ... Args>
    ComponentHandle<T> AddComponent(Args&& ... args);

    template <typename T>
    void RemoveComponent();

    template <typename T>
    bool HasComponent();

    template <typename Component>
    ComponentHandle<Component> Component();

    template <typename ... Components>
    std::tuple<ComponentHandle<Components>...> Components();

private:
    bool Valid() const { return m_ID != INVALID; }

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

class BaseComponentTypeSaver
{
public:
    virtual ~BaseComponentTypeSaver() {}
    virtual void RemoveComponent(Entity entity) = 0;
};

template <typename T>
class ComponentTypeSaver : public BaseComponentTypeSaver
{
public:
    void RemoveComponent(Entity entity) override
    {
        entity.RemoveComponent<T>();
    }
};

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
	std::vector<uint64_t> m_FreeList;
	std::vector<PoolAllocator*> m_ComponentPools;
    std::vector<BaseComponentTypeSaver*> m_ComponentTypes;

public:
    EntityManager(EventManager& eventManager) 
        : m_EventManager(eventManager) {}

    Entity CreateEntity();
    void Destroy(Entity::ID entity);

    template <typename T, typename ... Args>
    ComponentHandle<T> AddComponent(Entity::ID id, Args&& ... args);

    template <typename T>
    void RemoveComponent(Entity::ID id);

    template <typename T>
    ComponentHandle<T> Component(Entity::ID id);

    template <typename ... Components>
    std::tuple<ComponentHandle<Components>...> Components(Entity::ID id);

    template <typename ... Args>
    std::vector<Entity> EntitiesWithComponents();

    /*template <typename ...Args>
    std::vector<Entity> EntitiesWithComponents(Args... args);*/

    template <typename T>
    bool HasComponent(Entity::ID id) const;

private:

    /*template <typename T>
    bool HasComponentP(Entity::ID id, T t) const;

    template <typename T, typename ... Args>
    bool HasComponentP(Entity::ID id, T t, Args... args) const;*/

    template <typename T, typename ... Args>
    typename std::enable_if<sizeof...(Args) >= 1, bool>::type
    HasComponent(Entity::ID id) const;

    template <typename T>
    T* GetComponent(Entity::ID id);

    void AccomodateEntity(uint64_t index);

    template <typename T>
    ComponentPool<T>* AccomodateComponent();
};

template <typename T, typename ... Args>
ComponentHandle<T> EntityManager::AddComponent(Entity::ID id, Args&& ... args) 
{
    assert(Entity::Valid(id));

    const ComponentFamily::Family family = ComponentF<T>::GetFamily();
    assert(!m_EntityComponentMask[id.GetID()].test(family));

    ComponentPool<T>* pool = AccomodateComponent<T>();
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
    ComponentHandle<T> component(this, id);

    m_EntityComponentMask[id.GetID()].reset(family);

    pool->Destroy(index);
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

template <typename ... Components>
std::tuple<ComponentHandle<Components>...> EntityManager::Components(Entity::ID id)
{
    return std::make_tuple(Component<Components>(id)...);
}

/*template <typename ... Args>
std::vector<Entity> EntityManager::EntitiesWithComponents(Args ... args)
{
    std::vector<Entity> result;

    for (uint64_t i = 1; i < m_EntityComponentMask.size(); i++)
    {
        Entity::ID id(i);
        if (HasComponentP(Entity::ID(i), args...))
        {
            result.push_back(Entity(this, id));
        }
    }

    return result;
}*/

template <typename ... Args>
std::vector<Entity> EntityManager::EntitiesWithComponents()
{
    //std::cout << sizeof...(Args) << "\n";
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

/*template<typename T>
bool EntityManager::HasComponentP(Entity::ID id, T t) const
{
    return HasComponent<T>(id);
    return true;
}*/

/*template <typename T, typename ... Args>
bool EntityManager::HasComponentP(Entity::ID id, T t, Args... args) const
{
    return HasComponentP(id, t) && HasComponentP(id, args...);
}*/

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
ComponentPool<T>* EntityManager::AccomodateComponent()
{
    const ComponentFamily::Family family = ComponentF<T>::GetFamily();

    if (m_ComponentPools.size() <= family) 
    {
        m_ComponentPools.resize(family + 1);
    }

    if (!m_ComponentPools[family]) 
    {
        ComponentPool<T>* pool = new ComponentPool<T>();
        pool->Expand(m_IdCounter);
        m_ComponentPools[family] = pool;
    }

    if (m_ComponentTypes.size() <= family) 
    {
        m_ComponentTypes.resize(family + 1);
        ComponentTypeSaver<T>* type = new ComponentTypeSaver<T>();
        m_ComponentTypes[family] = type;
    }

    return static_cast<ComponentPool<T>*>(m_ComponentPools[family]);
}

//////////////////

template <typename T, typename ... Args>
ComponentHandle<T> Entity::AddComponent(Args&& ... args)
{
    return m_EntityManager->AddComponent<T>(m_ID, std::forward<Args>(args) ...);
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

template <typename ... Components>
std::tuple<ComponentHandle<Components>...> Entity::Components()
{
    return m_EntityManager->Components<Components>(m_ID);
}