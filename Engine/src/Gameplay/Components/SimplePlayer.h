#pragma once

#include <glm/glm.hpp>
#include <Gameplay/Bone.h>

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{

	private:

	public:
		std::vector<Bone> m_Bones;
		SimplePlayer() { gameObject = nullptr; }
        SimplePlayer(GameObject* go) 
		{ 
			gameObject = go; 
			model = go->GetComponent<Mesh>().Get()->m_Model;
			transform = go->GetComponent<Transform>().Get();
			
			model->m_FreeBones = true;

			for (int i = 0; i < model->m_BoneInfo.size(); ++i)
			{
				m_Bones.push_back(Bone(model->m_BoneInfo[i].GlobalTransformation));
				if (i > 0)
				{
					m_Bones[i].Init(&m_Bones[i - 1].m_Bone);
				}

				//std::cout<< model->m_BoneInfo[i].Name << std::endl;
				//m_Bones.push_back(Bone(model->m_BoneInfo[i].BoneOffset));
			}
		}

		void SimplePlayer::UpdateBones()
		{
			for (int i = 0; i < m_Bones.size(); i++)
			{			
				if (i > 0)
					m_Bones[i].UpdateBone(m_Bones[i-1].m_Bone.GetLocalPosition());
				else
					m_Bones[i].UpdateBone(glm::vec3(0.f));

			}
		}


        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}


		GameObject* gameObject;
		Model* model;
		Transform* transform;
		float air = 100;
	};

}