#pragma once

#include "Core/ISerializable.h"
#include "Renderer/Model.h"

namespace sixengine
{
	class Pistol : ISerializable
	{
	public:
		int m_BoneInfoIndex;
		Model* m_Model;
		Transform* m_OwnerTransform;

		Pistol(GameObject* go = nullptr, std::string boneName = "")
			: m_BoneInfoIndex(-1), m_Model(nullptr), m_OwnerTransform(nullptr)
		{
			if (go)
			{
				m_Model = go->GetComponent<Mesh>()->GetModel();
				m_OwnerTransform = go->GetComponent<Transform>().Get();
				m_BoneInfoIndex = go->GetComponent<Mesh>()->GetModel()->GetBoneInfoIndex(boneName);
			}
		}

		virtual void Load(std::iostream& stream) override {}
		virtual void Save(std::iostream& stream) override {}
	};
}