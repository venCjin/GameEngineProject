#pragma once

#include <glm/glm.hpp>
#include <Physics/Components/BoxCollider.h>
#include <Gameplay/Components/Transform.h>
#include <Physics/Collision.h>
#include <Physics/Components/BoxCollider.h>
#include <Physics/Components/DynamicBody.h>

#include "Core/CameraSystem/MixingCamera.h"

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{

	private:

	public:
		SimplePlayer() { gameObject = nullptr; }
        SimplePlayer(GameObject* go) 
		{ 
			gameObject = go;
			transform = go->GetComponent<Transform>().Get();
			//collider = go->GetComponent<BoxCollider>().Get();
		}
        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}


		GameObject* gameObject;
		Transform* transform;
		BoxCollider* collider;
		float air = 100;
		bool m_OnSurface = true;

		Material* scolopendraMaterial;
		Animation* scolopendraAnimation;

		Camera* OnSurfaceCamera;
		Camera* UnderSurfaceCamera;
		MixingCamera* MixingCamera;
	};

}