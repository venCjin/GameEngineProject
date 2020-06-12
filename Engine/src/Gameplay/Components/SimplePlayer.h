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
			model->m_FreeBones = true;
			for (int i = 0; i < model->m_BoneInfo.size(); ++i)
			{
				std::cout<< model->m_BoneInfo[i].Name << std::endl;
				m_Bones.push_back(Bone(model->m_BoneInfo[i].BoneOffset));

			}
		}
        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}


		GameObject* gameObject;
		Model* model;
		float air = 100;
	};

}