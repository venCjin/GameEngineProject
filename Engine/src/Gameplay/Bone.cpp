#pragma once

#include "pch.h"
#include "Bone.h"

namespace sixengine {

	Bone::Bone()
	{
		m_Bone = Transform();
	}

	Bone::Bone(glm::mat4 bone, std::string name)
	{
		m_Bone = Transform(bone);
		m_Name = name;
	}


	Bone::~Bone()
	{
	}

	void Bone::Init(Transform * parent)
	{
		m_Parent = parent;

		m_Length = glm::distance(m_Bone.GetLocalPosition(), m_Parent->GetLocalPosition());
		m_Inited = true;
	}

	void Bone::UpdateBone(glm::vec3 position)
	{
		if (m_Parent == nullptr || !m_Inited)
			return;		

		glm::vec3 direction = position - m_Bone.GetLocalPosition();
		direction = glm::normalize(direction);

		m_Bone.SetLocalPosition(position - (direction * m_Length));

		m_Bone.Rotate(glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		m_Bone.LookAtBone(position);
		m_Bone.Rotate(glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f))));

	}
}