#pragma once
#include <Gameplay/Components/Transform.h>

namespace sixengine {

	class Bone
	{
	public:
		Bone(); // czy potrzeba ustawiæ transform m_Bone?
		Bone(glm::mat4 bone);
		~Bone();

		Transform m_Bone;
		Transform* m_Parent;

		float m_Length;

		void UpdateBone();
	};

}