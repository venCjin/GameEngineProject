#pragma once

#include <glm/glm.hpp>
#include <Physics/Components/BoxCollider.h>
#include <Gameplay/Components/Transform.h>
#include <Physics/Collision.h>
#include <Physics/Components/BoxCollider.h>
#include <Physics/Components/DynamicBody.h>

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{
		SimplePlayer() { gameObject = nullptr; }
        SimplePlayer(GameObject* go) 
		{ 
			gameObject = go;
			transform = go->GetComponent<Transform>().Get();
			collider = go->GetComponent<BoxCollider>().Get();
		}
        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}
	public:
		GameObject* gameObject;
		Transform* transform;
		BoxCollider* collider;
		float air = 100;
		bool m_OnSurface = true;
	};

}