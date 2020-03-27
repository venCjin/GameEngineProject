#pragma once

#include <map>
namespace sixengine {
	class ResourceManager
	{
		// string is key for now, we will think about hashing it later
		std::map <std::string, float> m_Dictionary;

	public:
		ResourceManager();
		bool isResourceInDicktionary(std::string);
		bool AddResource(std::string key, float value);

	};

}

