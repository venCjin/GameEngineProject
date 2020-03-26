#pragma once

#include "entityx/Entity.h"

namespace sixengine {

	class GameObject
	{
	private:
		entityx::Entity m_Entity;
		//TODO: Add tags and other staff

	public:
		GameObject(entityx::EntityManager& entityManager) { m_Entity = entityManager.create(); }
		~GameObject() { m_Entity.destroy(); };

		template<typename T, typename... Args>
		void AddComponent(Args&& ... args)
		{
			m_Entity.assign<T>(std::forward<Args>(args) ... );
		}

		template<typename T>
		void AddComponent(const T& component)
		{
			m_Entity.assign(T);
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Entity.remove<T>();
		}

		template<typename T>
		entityx::ComponentHandle<T> GetComponent()
		{
			return m_Entity.component<T>();
		}
	};
}