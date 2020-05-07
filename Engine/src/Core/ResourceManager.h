#pragma once

#include <map>
#include <utility>
#include "Renderer/Shader.h"

namespace sixengine {

	template <typename T>
	class ResourceManager
	{
		// string is key for now, we will think about hashing it later
		std::map <std::string, T> m_Dictionary;

	public:
		ResourceManager();
		T Get(std::string key);
		bool IsResourceInDicktionary(std::string);
		bool AddResource(std::string key, T value);
		std::size_t Size();
	};

}

