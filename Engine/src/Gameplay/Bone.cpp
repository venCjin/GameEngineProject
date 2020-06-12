#pragma once

#include "pch.h"
#include "Bone.h"

namespace sixengine {

	Bone::Bone()
	{
	}

	Bone::Bone(glm::mat4 bone)
	{
		m_Bone = Transform(bone);
		std::cout << "pos: " << m_Bone.GetLocalPosition().x << " " << m_Bone.GetLocalPosition().y << " " << m_Bone.GetLocalPosition().z <<  std::endl;
	}


	Bone::~Bone()
	{
	}
	void Bone::UpdateBone()
	{
		LOG_INFO("Bone::UpdateBone()");
	}
}