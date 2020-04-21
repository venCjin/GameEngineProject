#pragma once

#include "EntityManager.h"

#include <vector>

class EventManager;

class BaseSystem
{
public:
	virtual void GetEntities(EntityManager& entityManager) = 0;
	virtual void UpdateAll(EventManager& eventManager, float dt) = 0;

	virtual void Update(EventManager& eventManager, float dt) = 0;
	virtual void OnStart(EventManager& eventManager);
};

#define EXPAND( x ) x

#define GET_MACRO(_1, _2, _3, _4, NAME, ...) NAME
#define CONSTRUCT_COMPONENTS(...) EXPAND(GET_MACRO(__VA_ARGS__, C_COMPONENT_4, C_COMPONENT_3, C_COMPONENT_2, C_COMPONENT_1)(__VA_ARGS__))
#define ASSIGN_COMPONENTS(...) EXPAND(GET_MACRO(__VA_ARGS__, A_COMPONENT_4, A_COMPONENT_3, A_COMPONENT_2, A_COMPONENT_1)(__VA_ARGS__))


#define C_COMPONENT_1(a) ComponentHandle<a> m_ ## a;

#define C_COMPONENT_2(a, b) C_COMPONENT_1(a)\
                          ComponentHandle<b> m_ ## b;

#define C_COMPONENT_3(a, b, c) C_COMPONENT_2(a, b)\
                             ComponentHandle<c> m_ ## c;

#define C_COMPONENT_4(a, b, c, d) C_COMPONENT_3(a, b, c)\
                             ComponentHandle<d> m_ ## d;


#define A_COMPONENT_1(a) m_ ## a = entity.Component<a>();

#define A_COMPONENT_2(a, b) A_COMPONENT_1(a)\
                          m_ ## b = entity.Component<b>();

#define A_COMPONENT_3(a, b, c) A_COMPONENT_2(a, b)\
                             m_ ## c = entity.Component<c>();

#define A_COMPONENT_4(a, b, c, d) A_COMPONENT_3(a, b, c)\
                             m_ ## d = entity.Component<d>();


#define SYSTEM(SystemName, ...)\
class Base ## SystemName : public BaseSystem\
{\
private:\
	std::vector<Entity> m_Entities;\
\
protected:\
	Entity entity;\
	CONSTRUCT_COMPONENTS(__VA_ARGS__)\
\
public:\
	void GetEntities(EntityManager& entityManager) override final\
	{\
		m_Entities = entityManager.EntitiesWithComponents<EXPAND(__VA_ARGS__)>();\
	}\
\
	void UpdateAll(EventManager& eventManager, float dt) override final\
	{\
		for (auto e : m_Entities)\
		{\
			entity = e;\
			ASSIGN_COMPONENTS(__VA_ARGS__)\
			Update(eventManager, dt);\
		}\
	}\
};\
class SystemName : public Base ## SystemName


class SystemManager
{
private:
	EntityManager& m_EntityManager;
	EventManager& m_EventManager;

	std::vector<BaseSystem*> m_Systems;

public:
	SystemManager(EntityManager& entityManager, EventManager& eventManager)
		: m_EntityManager(entityManager), m_EventManager(eventManager) {}

	template <typename T>
	void AddSystem();

	void UpdateAll(float dt);
};

template <typename T>
void SystemManager::AddSystem()
{
	T* system = new T();
	m_Systems.push_back(system);

	system->OnStart(m_EventManager);
}