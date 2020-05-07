#pragma once

#include "Renderer/Shader.h"
#include "Core/Camera.h"

namespace sixengine {

	struct SSBO
	{

	};

	class Technique
	{
	private:
		Shader* m_Shader;
		Camera* m_Camera;

		std::vector<SSBO> m_SSBOs;

	public:
		//Technique(Shader* shader, Camera* camera);
	};

}


