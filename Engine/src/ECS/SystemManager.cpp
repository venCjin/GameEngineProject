#pragma once

#include <pch.h>
#include <ECS/SystemManager.h>

void SystemManager::UpdateAll(float dt)
{
	for (auto s : m_Systems)
	{
		s->GetEntities(m_EntityManager);
		s->UpdateAll(m_EventManager, dt);
	}
}

void BaseSystem::OnStart(EventManager& eventManager)
{
}