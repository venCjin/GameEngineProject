#include "pch.h"
#include "Renderer/MaterialManager.h"

namespace sixengine
{
	MaterialManager* MaterialManager::instance = nullptr;
	MaterialManager* MaterialManager::getInstance()
	{
		if (instance == nullptr)
		{
			instance = new MaterialManager();
		}
		return instance;
	}
	Material* MaterialManager::CreateMaterial(Shader* shader, glm::vec4 textures, std::string materialName)
	{
		if (!this->IsResourceInDicktionary(materialName))
		{
			Material* material = new Material(shader, textures, materialName);
			this->AddResource(materialName, material);
		}

		return this->Get(materialName);
	}

}

