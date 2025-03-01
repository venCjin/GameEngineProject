#pragma once

#include <ECS/EntityManager.h>

namespace sixengine {

	class Transform;
	class Gizmo;

	class GameObject
	{
	private:
		Entity m_Entity;
		bool m_Dirty;
		std::vector<GameObject*> m_Childeren;
		std::vector<Gizmo*> m_Gizmos;
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
		void AddComponentCopy(const T& component)
		{
			m_Entity.AddComponentCopy<T>(std::forward<const T&>(component));
		}

		template<typename T>
		void RemoveComponent()
		{
			m_Entity.RemoveComponent<T>();
		}

		template<typename T>
		bool HasComponent()
		{
			return m_Entity.HasComponent<T>();
		}

		template<typename T>
		ComponentHandle<T> GetComponent()
		{
			return m_Entity.Component<T>();
		}

		template<typename T>
		bool GetFirstChildWithComponent(GameObject* result)
		{
			LOG_CORE_FATAL("GameObject::GetFirstChildWithComponent() DOES NOT WORKING");
			__debugbreak();

			if (this->HasComponent<T>())
			{
				result = this;
				return true;
			}

			for each (auto child in m_Childeren)
			{
				if (child->GetFirstChildWithComponent<T>(result)) return true;
			}

			return false;
		}

		template<typename T>
		void GetChildrenWithComponent(std::vector<GameObject*>& result)
		{
			if (this->HasComponent<T>()) result.push_back(this);

			for each (auto child in m_Childeren)
			{
				child->GetChildrenWithComponent<T>(result);
			}
		}

		// Scene graph

		void Render(bool first = false);

		void Render(Transform parentWorld, bool dirty);

		void OnDrawGizmos(bool first = false);

		void SetDirty(bool dirty);

		void AddChild(GameObject* child);

		void AddGizmo(Gizmo* gizmo);

		std::vector<GameObject*> GetChildren();
	#ifdef DEBUG
		void ImGuiWriteSceneTree();
		void ImGuiWriteSceneTreeNode();

		void ImGuiWriteUITree();
		void ImGuiWriteUITreeNode();
	#endif // DEBUG

	};

}