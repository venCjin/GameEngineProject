#pragma once

#include "entityx/Entity.h"

namespace sixengine {

	class Transform;

	class GameObject
	{
	private:
		entityx::Entity m_Entity;
		bool m_Dirty;
		std::vector<GameObject*> m_Childeren;
		//TODO: Add tags and other staff

	public:
		GameObject(entityx::EntityManager& entityManager) : m_Dirty(true)
		{
			m_Entity = entityManager.create();
		}
		
		~GameObject()
		{
			m_Entity.destroy();
			for (auto& go : m_Childeren) go->~GameObject();
		};

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

		// Scene graph

		void Render(glm::mat4 projection, glm::mat4 view);

		void Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty);

		void SetDirty(bool dirty);

		void AddChild(GameObject* child);

		std::vector<GameObject*> GetChildren();

	};
}