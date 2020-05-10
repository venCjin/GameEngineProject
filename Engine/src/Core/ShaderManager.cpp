#include "pch.h"
#include "ShaderManager.h"

namespace sixengine {

	Shader* ShaderManager::AddShader(std::string shaderPath)
	{

		std::size_t slash = shaderPath.find_last_of("/\\");
		std::size_t dot = shaderPath.find_last_of(".");
		std::string shaderName = shaderPath.substr(slash + 1, dot - slash - 1);

		if (!this->IsResourceInDicktionary(shaderName))
		{
			Shader* shader = new Shader(shaderPath.c_str());
			this->AddResource(shaderName, shader);
		}

		return this->Get(shaderName);
	}

}

