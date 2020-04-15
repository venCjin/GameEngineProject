#pragma once

#include <pch.h>
#include <ECS/SystemManager.h>

void SystemManager::UpdateAll(float dt)
{
	for (auto s : m_Systems)
		s->Update(m_EntityManager, m_EventManager, dt);
}