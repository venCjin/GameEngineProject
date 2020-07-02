#pragma once

#include "Core/ISerializable.h"
#include "Gameplay/Components/Mesh.h"
#include "Gameplay/GameObject.h"
#include "Gameplay/Components/LoopedSound.h"


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
			m_GameObject->GetComponent<Transform>()->Translate(0.0f, -100.0f, 0.0f);
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}