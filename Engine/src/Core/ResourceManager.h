#pragma once
#include <Renderer\Shader.h>
#include <map>
#include <utility>
namespace sixengine {
	template <typename T>
	class ResourceManager
	{
		// string is key for now, we will think about hashing it later
		std::map <std::string, T> m_Dictionary;

	public:
		ResourceManager();
		T get(std::string key);
		bool isResourceInDicktionary(std::string);
		bool AddResource(std::string key, T value);
		std::size_t size();
		//virtual T makeInstance(std::string key) = 0;
	};

}

