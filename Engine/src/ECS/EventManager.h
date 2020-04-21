#pragma once

#include <vector>
#include <functional>

#include <ECS/EntityManager.h>

//////////////////

class EventFamily
{
public:
	typedef uint64_t EFamily;

protected:
	static EFamily m_FamilyCounter;
};

template <typename T>
class EventF : public EventFamily
{
public:
	static EFamily GetFamily();
};

template <typename T>
EventFamily::EFamily EventF<T>::GetFamily()
{
	static EFamily family(m_FamilyCounter++);
	return family;
}

//////////////////

class BaseEvent
{
public:
	Entity::ID m_Id;

public:
	virtual ~BaseEvent() {}

protected:
	BaseEvent(Entity::ID id)
		: m_Id(id) {}
};

//////////////////

class EventManager
{
private:
	std::vector<std::list<std::function<void(BaseEvent&)>>> m_Listeners;

public:
	EventManager()
	{
		m_Listeners.resize(64);
	}

	template <typename T>
	inline void Emit(T baseEvent);

	template <typename T, typename S>
	inline void AddListener(void(S::*handle)(BaseEvent&), S* system);

	template <typename T, typename S>
	inline void RemoveListener(void(S::* handle)(BaseEvent&), S* system);
};

template<typename T>
inline void EventManager::Emit(T baseEvent)
{
	const EventFamily::EFamily family = EventF<T>::GetFamily();
	for (auto i : m_Listeners[family])
		i(baseEvent);
}

template<typename T, typename S>
inline void EventManager::AddListener(void(S::* handle)(BaseEvent&), S* system)
{
	std::function<void(BaseEvent&)> f = std::bind(handle, system, std::placeholders::_1);

	const EventFamily::EFamily family = EventF<T>::GetFamily();
	m_Listeners[family].push_back(f);
}

template<typename T, typename S>
inline void EventManager::RemoveListener(void(S::* handle)(BaseEvent&), S* system)
{
	std::function<void(BaseEvent&)> f = std::bind(handle, system, std::placeholders::_1);

	const EventFamily::EFamily family = EventF<T>::GetFamily();
	m_Listeners[family].remove(f);
}
