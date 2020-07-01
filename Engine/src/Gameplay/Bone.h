#pragma once
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	class Bone
	{
	private:
		Transform* m_Parent;
		bool m_Inited = false;
		float m_Length;

	public:
		Bone();
		Bone(glm::mat4 bone, std::string name);
		~Bone();

		std::string m_Name;
		void Init(Transform* parent);

		Transform m_Bone;

		void UpdateBone(glm::vec3 position);
	};

}