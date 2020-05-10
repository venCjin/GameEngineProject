#include "pch.h"
#include "Renderer/MaterialManager.h"

namespace sixengine
{

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

