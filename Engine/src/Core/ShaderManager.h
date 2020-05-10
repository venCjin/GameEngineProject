#pragma once

#include "ResourceManager.h"
#include "Renderer/Shader.h"

namespace sixengine {

	class ShaderManager : public ResourceManager<Shader*>
	{
	public:
		Shader* AddShader(std::string shaderPath);
	};

}


