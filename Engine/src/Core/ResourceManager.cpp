#include "pch.h"
#include "Core/ResourceManager.h"

namespace sixengine {

	template <typename T>
	ResourceManager<T>::ResourceManager()
	{
		this->m_Dictionary = std::map<std::string, T>();
	}

	template<typename T>
	T ResourceManager<T>::Get(std::string key)
	{
		return m_Dictionary[key];
	}

	template <typename T>
	bool ResourceManager<T>::IsResourceInDicktionary(std::string key)
	{
		return m_Dictionary.find(key) != m_Dictionary.end();
	}

	template <typename T>
	bool ResourceManager<T>::AddResource(std::string key, T value)
	{
		std::pair<std::string, T> pair = std::pair(key, value);
		bool flag = m_Dictionary.insert(pair).second;
		return flag;
	}

	template<typename T>
	std::size_t ResourceManager<T>::Size()
	{
		return m_Dictionary.size();
	}

	template class ResourceManager<float>;
	template class ResourceManager<class Shader*>;
	template class ResourceManager<class Material*>;
	template class ResourceManager<std::string>;

}
