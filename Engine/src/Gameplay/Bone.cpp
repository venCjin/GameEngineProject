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
		//glm::vec3 r = m_GlobalTransformationBone.GetLocalOrientation();
		//std::cout << "start rot: " << r.x << " " << r.y << " " << r.z << std::endl;
	}


	Bone::~Bone()
	{
	}

	void Bone::Init(Transform * parent)
	{
		m_Parent = parent;

		//TODO: m_LocalPosition

		m_Length = glm::distance(m_GlobalTransformationBone.GetLocalPosition(), m_Parent->GetLocalPosition());
		m_Inited = true;
	}

	void Bone::UpdateBone(Bone* bone)
	{
		if (m_Parent == nullptr || !m_Inited)
			return;		

		glm::vec3 position = glm::vec3(0.f);
		if (bone != nullptr)
			position = bone->m_GlobalTransformationBone.GetLocalPosition() + bone->m_LocalTransformationBone.GetLocalPosition();

		glm::vec3 thisBoneWorldPosition = m_GlobalTransformationBone.GetLocalPosition() + m_LocalTransformationBone.GetLocalPosition();

		glm::vec3 direction = position - thisBoneWorldPosition;
		direction = glm::normalize(direction);

		m_LocalTransformationBone.SetLocalPosition(position - (direction * m_Length) - m_GlobalTransformationBone.GetLocalPosition());

		m_LocalTransformationBone.Rotate(glm::quat(glm::vec3(glm::radians(-90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		m_LocalTransformationBone.LookAtBone(position);
		m_LocalTransformationBone.Rotate(glm::quat(glm::vec3(glm::radians(90.0f), glm::radians(0.0f), glm::radians(0.0f))));
		

	}
}