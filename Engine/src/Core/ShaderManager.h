#pragma once

#include "ResourceManager.h"

namespace sixengine {

	class ShaderManager : public ResourceManager<Shader*>
	{
	public:
		Shader* makeInstance(std::string vertPath, std::string fragPath);
	};

}


