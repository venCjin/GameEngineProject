#pragma once

#include "ResourceManager.h"

namespace sixengine {

	class ShaderManager : public ResourceManager<Shader*>
	{
	public:
		Shader* AddShader(std::string shaderPath);
	};

}


