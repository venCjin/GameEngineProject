#pragma once

#include "EntityManager.h"

#include <vector>

class System
{
public:
	virtual void Update(EntityManager& entityManager, EventManager& eventManager, float dt) = 0;
};

class SystemManager 
{
private:
	EntityManager& m_EntityManager;
	EventManager& m_EventManager;

	std::vector<System*> m_Systems;

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
	m_Systems.push_back(new T());
}
