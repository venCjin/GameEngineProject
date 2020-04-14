#pragma once

#include <glm/glm.hpp>
#include <map>

#include "Core/Events/Event.h"
#include "Core/Events/KeyEvent.h"
#include "Core/Events/MouseEvent.h"

#include "KeyCode.h"

#define BINDABLE template <class _Func, class _Object>

namespace sixengine {

	enum Axis {
		KEYBOARD_HORIZONTAL,
		KEYBOARD_VERTICAL,
	};

	class Input
	{
	public:
		/// Dodaæ komentarze

		static Input* Get();
		static void Release();

		void OnPreUpdate();
		void OnPostUpdate();
		void OnEvent(Event& event);

		static bool IsAnyKeyActive();
		
		static bool IsKeyActive(KeyCode keyCode);
		static bool IsKeyPressed(KeyCode keyCode);
		static bool IsKeyReleased(KeyCode keyCode);
		
		static float GetAxis(Axis axis);
		static glm::vec3 GetMovementAxis();

		BINDABLE static void AddKeyActiveBinding(KeyCode keyCode, void(_Func::* function)(), _Object object);
		BINDABLE static void AddKeyPressedBinding(KeyCode keyCode, void(_Func::* function)(), _Object object);
		BINDABLE static void AddKeyReleasedBinding(KeyCode keyCode, void(_Func::* function)(), _Object object);

		BINDABLE static void AddKeyContinousBinding(KeyCode keyCode, void (_Func::* function)(bool), _Object object);
		BINDABLE static void AddKeyChangedBinding(KeyCode keyCode, void (_Func::* function)(bool), _Object object);
		
		BINDABLE static void AddAxisBinding(Axis axis, void (_Func::* function)(float), _Object object);
		BINDABLE static void AddMovementAxisBinding(void (_Func::* function)(glm::vec3), _Object object);



		static bool IsMouseButtonActive(int button);
		static bool IsMouseButtonPressed(int button);
		static bool IsMouseButtonReleased(int button);

		static float GetMouseDeltaPosX();
		static float GetMouseDeltaPosY();
		static glm::vec2 GetMouseDeltaPos();


		BINDABLE static void AddMouseButtonActiveBinding(int button, void(_Func::* function)(), _Object object);
		BINDABLE static void AddMouseButtonPressedBinding(int button, void(_Func::* function)(), _Object object);
		BINDABLE static void AddMouseButtonReleasedBinding(int button, void(_Func::* function)(), _Object object);

		BINDABLE static void AddMouseButtonContinousBinding(int button, void(_Func::* function)(), _Object object);
		BINDABLE static void AddMouseButtonChangedBinding(int button, void(_Func::* function)(), _Object object);

		BINDABLE static void AddMousePositionBinding(void(_Func::* function)(float, float), _Object object);
		BINDABLE static void AddMouseDeltaPosBinding(void(_Func::* function)(float, float), _Object object);


	private:

		static Input* m_InputSystemInstance;

		std::unordered_set<KeyCode> m_currentKeys;
		std::unordered_set<KeyCode> m_pressedKeys;
		std::unordered_set<KeyCode> m_releasedKeys;

		std::list<std::pair<KeyCode, std::function<void()>>> m_keyActiveBindings;
		std::list<std::pair<KeyCode, std::function<void()>>> m_keyPressedBindings;
		std::list<std::pair<KeyCode, std::function<void()>>> m_keyReleasedBindings;

		std::list<std::pair<KeyCode, std::function<void(bool)>>> m_keyContinouesBindings;
		std::list<std::pair<KeyCode, std::function<void(bool)>>> m_keyChangedBindings;

		std::list<std::function<void(glm::vec3)>> m_movementAxisBindings;
		std::list<std::pair<Axis, std::function<void(float)>>> m_axisBindings;


		std::unordered_set<int> m_currentMouseButtons;
		std::unordered_set<int> m_pressedMouseButtons;
		std::unordered_set<int> m_releasedMouseButtons;

		std::list<std::pair<int, std::function<void()>>> m_MouseButtonActiveBindings;
		std::list<std::pair<int, std::function<void()>>> m_MouseButtonPressedBindings;
		std::list<std::pair<int, std::function<void()>>> m_MouseButtonReleasedBindings;

		std::list<std::pair<int, std::function<void(bool)>>> m_MouseButtonContinouesBindings;
		std::list<std::pair<int, std::function<void(bool)>>> m_MouseButtonChangedBindings;

		std::list<std::function<void(float, float)>> m_mousePositionBindings;
		std::list<std::function<void(float, float)>> m_mouseDeltaPosBindings;

		float m_currentMousePosX = 0.0f;
		float m_currentMousePosY = 0.0f;

		float m_lastMousePosX = 0.0f;
		float m_lastMousePosY = 0.0f;


		Input();
		~Input();

		bool OnKeyPressed(KeyPressedEvent& event);
		bool OnKeyReleased(KeyReleasedEvent& event);

		bool OnMouseMoved(MouseMovedEvent& event);
		bool OnMouseButtonPressed(MouseButtonPressedEvent& event);
		bool OnMouseButtonReleased(MouseButtonReleasedEvent& event);

		bool IsAnyKeyActiveImpl();

		bool IsKeyActiveImpl(KeyCode keyCode);
		bool IsKeyPressedImpl(KeyCode keyCode);
		bool IsKeyReleasedImpl(KeyCode keyCode);

		float GetAxisImpl(Axis axis);
		glm::vec3 GetMovementAxisImpl();


		bool IsMouseButtonActiveImpl(int button);
		bool IsMouseButtonPressedImpl(int button);
		bool IsMouseButtonReleasedImpl(int button);

		float GetMouseDeltaPosXImpl();
		float GetMouseDeltaPosYImpl();
		glm::vec2 GetMouseDeltaPosImpl();
	};


	BINDABLE
	inline void Input::AddKeyActiveBinding(KeyCode keyCode, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_keyActiveBindings.push_back(std::make_pair(keyCode, binding));
	}

	BINDABLE
	inline void Input::AddKeyPressedBinding(KeyCode keyCode, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_keyPressedBindings.push_back(std::make_pair(keyCode, binding));
	}

	BINDABLE
	inline void Input::AddKeyReleasedBinding(KeyCode keyCode, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_keyReleasedBindings.push_back(std::make_pair(keyCode, binding));
	}

	BINDABLE
	inline void Input::AddKeyContinousBinding(KeyCode keyCode, void(_Func::* function)(bool), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1);
		Get()->m_keyContinouesBindings.push_back(std::make_pair(keyCode, binding));
	}

	BINDABLE
	inline static void Input::AddKeyChangedBinding(KeyCode keyCode, void(_Func::* function)(bool), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1);
		Get()->m_keyChangedBindings.push_back(std::make_pair(keyCode, binding));
	}
	
	BINDABLE
	inline void Input::AddAxisBinding(Axis axis, void(_Func::* function)(float), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1);
		Get()->m_axisBindings.push_back(std::make_pair(axis, binding));
	}

	BINDABLE
	inline void Input::AddMovementAxisBinding(void(_Func::* function)(glm::vec3), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1);
		Get()->m_movementAxisBindings.push_back(binding);
	}

	BINDABLE
	inline void Input::AddMouseButtonActiveBinding(int button, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_MouseButtonActiveBindings.push_back(std::make_pair(button, binding));
	}
	
	BINDABLE
	inline void Input::AddMouseButtonPressedBinding(int button, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_MouseButtonPressedBindings.push_back(std::make_pair(button, binding));
	}
	
	BINDABLE
	inline void Input::AddMouseButtonReleasedBinding(int button, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_MouseButtonReleasedBindings.push_back(std::make_pair(button, binding));
	}

	BINDABLE
	inline void Input::AddMouseButtonContinousBinding(int button, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_MouseButtonContinouesBindings.push_back(std::make_pair(button, binding));
	}
	
	BINDABLE
	inline void Input::AddMouseButtonChangedBinding(int button, void(_Func::* function)(), _Object object)
	{
		auto binding = std::bind(function, object);
		Get()->m_MouseButtonChangedBindings.push_back(std::make_pair(button, binding));
	}
	
	BINDABLE
	inline static void Input::AddMousePositionBinding(void(_Func::* function)(float, float), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1, std::placeholders::_2);
		Get()->m_mousePositionBindings.push_back(binding);
	}
	
	BINDABLE
	inline static void Input::AddMouseDeltaPosBinding(void(_Func::* function)(float, float), _Object object)
	{
		auto binding = std::bind(function, object, std::placeholders::_1, std::placeholders::_2);
		Get()->m_mouseDeltaPosBindings.push_back(binding);
	}

}

