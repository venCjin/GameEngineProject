#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/LoopedSound.h"


namespace sixengine
{
	class Generator : ISerializable
	{
	public:
		Generator(Model* model, GameObject* go = nullptr)
			: destroyed(false), secondModel(model), owner(go) {}

		bool destroyed;
		Model* secondModel;
		GameObject* owner;

		void DestroyGenerator()
		{
			//setting rad to -1 so it would always be paused
			owner->GetComponent<LoopedSound3D>()->SetRad(-1.0f);
			//owner->RemoveComponent<LoopedSound3D>();
			owner->GetComponent<Mesh>()->m_Model = secondModel;
			irrklang::ISound* s = INIT_TRACK_3D("explosion", owner->GetComponent<Transform>()->GetWorldPosition());
			s->setIsPaused(false);
			destroyed = true;
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}