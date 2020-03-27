#include "pch.h"
#include "ResourceManager.h"

namespace sixengine {

	inline ResourceManager::ResourceManager()
	{
		this->m_Dictionary = std::map< std::string, float>();
	}


	bool ResourceManager::isResourceInDicktionary(std::string key)
	{
		//return m_Dictionary.find(key);
		return true;
	}


	bool ResourceManager::AddResource(std::string key, float value)
	{

		//bool flag = m_Dictionary.insert(std::makepair(key, value)).second;
		return true;
	}

}
