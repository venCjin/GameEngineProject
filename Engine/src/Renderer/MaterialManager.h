#pragma once

#include "Core/ResourceManager.h"
#include "Renderer/Material.h"

#include <string>

namespace sixengine {

	class MaterialManager : public ResourceManager<Material*>
	{
	public:
		Material* CreateMaterial(Shader* shader, glm::vec4 textures, std::string materialName);
	};

}

