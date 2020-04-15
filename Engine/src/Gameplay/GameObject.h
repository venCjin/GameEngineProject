#pragma once

#include <ECS/EntityManager.h>

namespace sixengine {

	class Transform;

	class GameObject
	{
	private:
		Entity m_Entity;
		bool m_Dirty;
		std::vector<GameObject*> m_Childeren;
		//TODO: Add tags and other staff

	public:
		GameObject(EntityManager& entityManager) : m_Dirty(true)
		{
			m_Entity = entityManager.CreateEntity();
		}
		
		~GameObject()
		{
			m_Entity.Destroy();
			for (auto& go : m_Childeren) 
				go->~GameObject();
		};

		template<typename T, typename... Args>
		void AddComponent(Args&& ... args)
		{
			m_Entity.AddComponent<T>(std::forward<Args>(args) ... );
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Entity.Remove<T>();
		}

		template<typename T>
		ComponentHandle<T> GetComponent()
		{
			return m_Entity.Component<T>();
		}

		// Scene graph

		void Render(glm::mat4 projection, glm::mat4 view);

		void Render(glm::mat4 projection, glm::mat4 view, Transform parentWorld, bool dirty);

		void SetDirty(bool dirty);

		void AddChild(GameObject* child);

		std::vector<GameObject*> GetChildren();
	};

}