#pragma once

#include "pch.h"
#include "Bone.h"

namespace sixengine {

	Bone::Bone()
	{
		m_GlobalTransformationBone = Transform();
	}

	Bone::Bone(glm::mat4 bone)
	{
		m_GlobalTransformationBone = Transform(bone);
	}


	Bone::~Bone()
	{
	}

	void Bone::Init(Transform * parent)
	{
		m_Parent = parent;

		m_Length = glm::distance(m_GlobalTransformationBone.GetLocalPosition(), m_Parent->GetLocalPosition());
		m_Inited = true;
	}

	void Bone::UpdateBone(glm::vec3 position)
	{
		if (m_Parent == nullptr || !m_Inited)
			return;		

		glm::vec3 direction = position - m_GlobalTransformationBone.GetLocalPosition();
		direction = glm::normalize(direction);

		m_GlobalTransformationBone.SetLocalPosition(position - (direction * m_Length));

		m_GlobalTransformationBone.Rotate(glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		m_GlobalTransformationBone.LookAtBone(position);
		m_GlobalTransformationBone.Rotate(glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f))));

	}
}