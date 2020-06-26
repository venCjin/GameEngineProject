#pragma once
#include "pch.h"

#include <glm/glm.hpp>
#include <Gameplay/Bone.h>
#include <Renderer/Model.h>
#include <Gameplay/Components/Mesh.h>
#include "Gameplay/GameObject.h"

#include <iostream>     // std::cout
#include <algorithm>    // std::sort
#include <vector>       // std::vector


namespace sixengine {

	class ScolopendraComponent
	{
	public:
		GameObject* m_GameObject;
		Transform* m_PlayerTransform;
		Model* m_Model;
		float air = 100;

		std::map<int, Bone> m_Bones;
		ScolopendraComponent() { m_PlayerTransform = nullptr; }

		// We need   GameObject* player   only for transform - scolopendra needs to be seperate GameObject
		ScolopendraComponent(GameObject* go, GameObject* player)
		{
			m_GameObject = go;
			m_Model = go->GetComponent<Mesh>().Get()->m_Model;
			m_PlayerTransform = player->GetComponent<Transform>().Get();

			m_Model->SetFrustumInfinity();

			for (int i = 0; i < m_Model->m_BoneInfo.size(); ++i)
			{
				std::size_t found = m_Model->m_BoneInfo[i].Name.find("Bone");
				if (found != std::string::npos)
				{
					m_Model->m_BoneInfo[i].FreeBone = true;
					if (m_Bones.find(i) == m_Bones.end())
					{
						m_Bones[i] = Bone(m_Model->m_BoneInfo[i].GlobalTransformation);
						if (i > 0)
							m_Bones[i].Init(&m_Bones[i - 1].m_GlobalTransformationBone);
					}					
				}
			}			
		}

		void ScolopendraComponent::UpdateBones()
		{

			for (int i = 0; i < m_Bones.size(); i++)
			{
				if (i > 0)
					m_Bones[i].UpdateBone(&m_Bones[i - 1]);
				else
					m_Bones[i].UpdateBone(nullptr);

			}
		}
	};
}
