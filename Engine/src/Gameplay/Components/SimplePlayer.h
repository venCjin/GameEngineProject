#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{
		SimplePlayer() { gameObject = nullptr; }
        SimplePlayer(GameObject* go) { gameObject = go; }
        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}
	public:
		GameObject* gameObject;
		float air = 100;
		bool m_OnSurface = true;
	};

}