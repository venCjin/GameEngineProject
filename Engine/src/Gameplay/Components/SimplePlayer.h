#pragma once

#include <glm/glm.hpp>

namespace sixengine {

	struct SimplePlayer : public ISerializable
	{

	private:

	public:
		SimplePlayer() { gameObject = nullptr; }
        SimplePlayer(GameObject* go) 
		{ 
			gameObject = go; 
			transform = go->GetComponent<Transform>().Get();			
		}
		

        virtual void Load(std::iostream& stream) {}
		virtual void Save(std::iostream& stream) {}


		GameObject* gameObject;
		Transform* transform;
		float air = 100;
	};

}