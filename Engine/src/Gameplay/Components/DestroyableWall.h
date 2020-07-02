#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/LoopedSound.h"
#include <random>


namespace sixengine
{
	class DestroyableWall : ISerializable
	{
	public:
		GameObject* m_GameObject;
		bool m_Destroyed;
		
		DestroyableWall(GameObject* go = nullptr)
		{
			m_GameObject = go;
			m_Destroyed = false;
		}

		void Destroy()
		{
			m_GameObject->RemoveComponent<BoxCollider>();
			m_Destroyed = true;

			for (auto child : m_GameObject->GetComponent<Transform>()->GetChildren())
			{
				std::random_device dev;
				std::mt19937 rng(dev());
				std::uniform_real_distribution<> dist(0.0f, 1.5f);

				auto offset = child->GetWorldPosition() - Application::Get().GetEntityManager()->EntitiesWithComponents<SimplePlayer>()[0].Component<Transform>()->GetWorldPosition();
				offset.y = 0.0f;

				auto r = dist(rng);
				LOG_INFO(r);

				child->Translate(glm::normalize(offset) * (float)r);

				std::uniform_real_distribution<> rotation(0.0f, 90.0f);

				child->Rotate(rotation(rng), rotation(rng), rotation(rng));
			}
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}