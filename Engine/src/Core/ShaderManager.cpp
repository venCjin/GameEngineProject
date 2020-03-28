#include "pch.h"
#include "ShaderManager.h"


namespace sixengine
{
	Shader* ShaderManager::makeInstance(std::string vertPath, std::string fragPath)
	{
		std::string key = vertPath + fragPath;
		if (!this->isResourceInDicktionary(key))
		{
			Shader* shader = new Shader(vertPath.c_str(), fragPath.c_str());
			this->AddResource(key, shader);
		}
		return this->get(key);
	}
}

