#pragma once

#include "pch.h"
#include "Bone.h"

namespace sixengine {

	Bone::Bone()
	{
		m_Bone = Transform();
	}

	Bone::Bone(glm::mat4 bone)// : m_Bone(bone)
	{
		m_Bone = Transform(bone);
		LOG_INFO("Construct");

		//std::cout << "pos: " << m_Bone.GetLocalPosition().x << " " << m_Bone.GetLocalPosition().y << " " << m_Bone.GetLocalPosition().z <<  std::endl;
		//std::cout << "rot: " << m_Bone.GetLocalOrientation().x << " " << m_Bone.GetLocalOrientation().y << " " << m_Bone.GetLocalOrientation().z <<  std::endl;
	}


	Bone::~Bone()
	{
	}

	void Bone::Init(Transform * parent)
	{
		m_Parent = parent;

		m_Length = glm::distance(m_Bone.GetLocalPosition(), m_Parent->GetLocalPosition());
		m_Inited = true;
		LOG_INFO("InitBone, length: {0}", m_Length);

	}

	void Bone::UpdateBone(glm::vec3 position)
	{

		LOG_INFO("UpdateBone");
		if (m_Parent == nullptr || !m_Inited)
			return;

		glm::vec3 f = position;//m_Parent->GetLocalPosition();
		LOG_CORE_INFO("Parent pos {0} {1} {2}", f.x, f.y, f.z);
		

		glm::vec3 direction = position -m_Bone.GetLocalPosition();
		direction = glm::normalize(direction);

		m_Bone.SetLocalPosition(position- (direction * m_Length));

		m_Bone.Rotate(glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		m_Bone.LookAtBone(position);
		m_Bone.Rotate(glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		
		//f = temp1.GetForward();
		//LOG_CORE_INFO("Bone direction AFTER ROTATION {0} {1} {2}", f.x, f.y, f.z);

	}
}