#include "pch.h"
#include "Input.h"

#include "Core.h"
#include "Application.h"

class Application;

namespace sixengine 
{
	Input* Input::m_InputSystemInstance = nullptr;

	Input::Input()
	{

	}
	
	Input::~Input()
	{

	}

	Input* Input::Get()
	{
		if (m_InputSystemInstance == nullptr)
		{
			m_InputSystemInstance = new Input();
		}

		return m_InputSystemInstance;
	}
	
	void Input::Release()
	{
		delete m_InputSystemInstance;
		m_InputSystemInstance = nullptr;
	}

	void Input::OnPreUpdate()
	{
		/// KEYBOARD ///
		for (auto key : m_releasedKeys)
		{
			for (auto bindingPair : m_keyReleasedBindings)
			{
				if (bindingPair.first == key)
				{
					bindingPair.second();
				}
			}

			for (auto bindingPair : m_keyChangedBindings)
			{
				if (bindingPair.first == key)
				{
					bindingPair.second(false);
				}
			}

			m_currentKeys.erase(key);
		}

		for (auto bindingPair : m_keyContinouesBindings)
		{
			bindingPair.second(IsKeyActiveImpl(bindingPair.first));
		}

		for (auto bindingPair : m_keyActiveBindings)
		{
			if (IsKeyActiveImpl(bindingPair.first))
			{
				bindingPair.second();
			}
		}

		glm::vec3 movementAxis = GetMovementAxisImpl();
		for (auto binding : m_movementAxisBindings)
		{
			binding(movementAxis);
		}

		for (auto bindingPair : m_axisBindings)
		{
			bindingPair.second(GetAxisImpl(bindingPair.first));
		}
		///


		/// MOUSE ///
		for (auto button : m_releasedMouseButtons)
		{
			for (auto bindingPair : m_MouseButtonReleasedBindings)
			{
				if (bindingPair.first == button)
				{
					bindingPair.second();
				}
			}

			for (auto bindingPair : m_MouseButtonChangedBindings)
			{
				if (bindingPair.first == button)
				{
					bindingPair.second(false);
				}
			}

			m_currentMouseButtons.erase(button);
		}

		for (auto bindingPair : m_MouseButtonContinouesBindings)
		{
			bindingPair.second(IsMouseButtonActiveImpl(bindingPair.first));
		}

		for (auto bindingPair : m_MouseButtonActiveBindings)
		{
			if (IsMouseButtonActiveImpl(bindingPair.first))
			{
				bindingPair.second();
			}
		}

		for (auto binding : m_mousePositionBindings)
		{
			binding(m_currentMousePosX, m_currentMousePosY);
		}

		for (auto binding : m_mouseDeltaPosBindings)
		{
			binding(GetMouseDeltaPosXImpl(), GetMouseDeltaPosYImpl());
		}
	}

	void Input::OnPostUpdate()
	{
		m_pressedKeys.clear();
		m_releasedKeys.clear();

		m_pressedMouseButtons.clear();
		m_releasedMouseButtons.clear();

		m_lastMousePosX = m_currentMousePosX;
		m_lastMousePosY = m_currentMousePosY;
	}

	void Input::OnEvent(Event& event)
	{
		EventDispatcher dispatcher(event);

		dispatcher.Dispatch<KeyPressedEvent>(BIND_EVENT_FN(Input::OnKeyPressed));
		dispatcher.Dispatch<KeyReleasedEvent>(BIND_EVENT_FN(Input::OnKeyReleased));

		dispatcher.Dispatch<MouseMovedEvent>(BIND_EVENT_FN(Input::OnMouseMoved));
		dispatcher.Dispatch<MouseButtonPressedEvent>(BIND_EVENT_FN(Input::OnMouseButtonPressed));
		dispatcher.Dispatch<MouseButtonReleasedEvent>(BIND_EVENT_FN(Input::OnMouseButtonReleased));
	}

	bool Input::IsAnyKeyActive()
	{
		return Input::Get()->IsAnyKeyActiveImpl();
	}

	bool Input::IsAnyKeyActiveImpl()
	{
		return (m_currentKeys.size() > 0);
	}

	bool Input::IsKeyActive(KeyCode keyCode)
	{
		return Input::Get()->IsKeyActiveImpl(keyCode);
	}

	bool Input::IsKeyActiveImpl(KeyCode keyCode)
	{
		return (m_currentKeys.find(keyCode) != m_currentKeys.end());
	}

	bool Input::IsKeyPressed(KeyCode keyCode)
	{
		return Get()->IsKeyPressedImpl(keyCode);
	}

	bool Input::IsKeyPressedImpl(KeyCode keyCode)
	{
		return (m_pressedKeys.find(keyCode) != m_pressedKeys.end());
	}

	bool Input::IsKeyReleased(KeyCode keyCode)
	{
		return Get()->IsKeyReleasedImpl(keyCode);
	}

	bool Input::IsKeyReleasedImpl(KeyCode keyCode)
	{
		return (m_releasedKeys.find(keyCode) != m_releasedKeys.end());
	}

	float Input::GetAxis(Axis axis)
	{
		return Get()->GetAxisImpl(axis);
	}

	float Input::GetAxisImpl(Axis axis)
	{
		if (axis == Axis::KEYBOARD_HORIZONTAL)
		{
			float result = 0.0f;
			result += (IsKeyActiveImpl(KeyCode::D)) ? (1.0f) : (0.0f);
			result -= (IsKeyActiveImpl(KeyCode::A)) ? (1.0f) : (0.0f);
			return result;
		}

		if (axis == Axis::KEYBOARD_VERTICAL)
		{
			float result = 0.0f;
			result += (IsKeyActiveImpl(KeyCode::W)) ? (1.0f) : (0.0f);
			result -= (IsKeyActiveImpl(KeyCode::S)) ? (1.0f) : (0.0f);
			return result;
		}

		return 0.0f;
	}

	glm::vec3 Input::GetMovementAxis()
	{
		return Get()->GetMovementAxisImpl();
	}

	glm::vec3 Input::GetMovementAxisImpl()
	{
		glm::vec3 axis;
		
		axis.x =  GetAxisImpl(Axis::KEYBOARD_HORIZONTAL);
		axis.y =  0.0f;
		axis.z = -GetAxisImpl(Axis::KEYBOARD_VERTICAL);
		
		if (glm::length(axis) > 0.0f)
		{
			axis = glm::normalize(axis);
		}

		return axis;
	}

	bool Input::IsMouseButtonActive(int button)
	{
		return Get()->IsMouseButtonActiveImpl(button);
	}

	bool Input::IsMouseButtonActiveImpl(int button)
	{
		return (m_currentMouseButtons.find(button) != m_currentMouseButtons.end());
	}

	bool Input::IsMouseButtonPressed(int button)
	{
		return Get()->IsMouseButtonPressedImpl(button);
	}

	bool Input::IsMouseButtonPressedImpl(int button)
	{
		return (m_pressedMouseButtons.find(button) != m_pressedMouseButtons.end());
	}

	bool Input::IsMouseButtonReleased(int button)
	{
		return Get()->IsMouseButtonReleasedImpl(button);
	}

	bool Input::IsMouseButtonReleasedImpl(int button)
	{
		return (m_releasedMouseButtons.find(button) != m_releasedMouseButtons.end());
	}

	float Input::GetMouseDeltaPosX()
	{
		return Get()->GetMouseDeltaPosXImpl();
	}

	float Input::GetMouseDeltaPosXImpl()
	{
		return (m_currentMousePosX - m_lastMousePosX);
	}

	float Input::GetMouseDeltaPosY()
	{
		return Get()->GetMouseDeltaPosYImpl();
	}

	float Input::GetMouseDeltaPosYImpl()
	{
		return (m_currentMousePosY - m_lastMousePosY);
	}

	glm::vec2 Input::GetMouseDeltaPos()
	{
		return Get()->GetMouseDeltaPosImpl();
	}

	glm::vec2 Input::GetMouseDeltaPosImpl()
	{
		return glm::vec2(GetMouseDeltaPosXImpl(), GetMouseDeltaPosYImpl());
	}
	   
	bool Input::OnKeyPressed(KeyPressedEvent& event)
	{
		if (IsKeyActiveImpl((KeyCode)(event.GetKeyCode())) == false)
		{
			m_currentKeys.insert((KeyCode)(event.GetKeyCode()));
			m_pressedKeys.insert((KeyCode)(event.GetKeyCode()));
		
			for (auto bindingPair : m_keyPressedBindings)
			{
				if (bindingPair.first == (KeyCode)(event.GetKeyCode()))
				{
					bindingPair.second();
				}
			}

			for (auto bindingPair : m_keyChangedBindings)
			{
				if (bindingPair.first == (KeyCode)(event.GetKeyCode()))
				{
					bindingPair.second(true);
				}
			}
		}

		return true;
	}

	bool Input::OnKeyReleased(KeyReleasedEvent& event)
	{
		m_releasedKeys.insert((KeyCode)(event.GetKeyCode()));
		return true;
	}

	bool Input::OnMouseMoved(MouseMovedEvent& event)
	{
		m_currentMousePosX = event.GetX();
		m_currentMousePosY = event.GetY();

		return true;
	}
	
	bool Input::OnMouseButtonPressed(MouseButtonPressedEvent& event)
	{
		if (IsMouseButtonActiveImpl(event.GetMouseButton()) == false)
		{
			m_currentMouseButtons.insert(event.GetMouseButton());
			m_pressedMouseButtons.insert(event.GetMouseButton());

			for (auto bindingPair : m_MouseButtonPressedBindings)
			{
				if (bindingPair.first == event.GetMouseButton())
				{
					bindingPair.second();
				}
			}

			for (auto bindingPair : m_MouseButtonChangedBindings)
			{
				if (bindingPair.first == event.GetMouseButton())
				{
					bindingPair.second(true);
				}
			}
		}

		return true;
	}
	
	bool Input::OnMouseButtonReleased(MouseButtonReleasedEvent& event)
	{
		m_releasedMouseButtons.insert(event.GetMouseButton());
		return true;
	}
}