#pragma once

#include <map>
#include <utility>

namespace sixengine {

	template <typename T>
	class ResourceManager
	{
		std::map<std::string, T> m_Dictionary;

	public:
		ResourceManager();
		T Get(std::string key);
		bool IsResourceInDicktionary(std::string);
		bool AddResource(std::string key, T value);
		std::size_t Size();
	};

}

