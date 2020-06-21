#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"


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
			owner->GetComponent<Mesh>()->m_Model = secondModel;
			destroyed = true;
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}