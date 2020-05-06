#pragma once

#include "Gameplay/GameObject.h"
#include "Core/Camera.h"
#include "Core/ShaderManager.h"

namespace sixengine {

	class Scene
	{
	public:
		GameObject *m_UI, *m_Scene;
		Camera cam, camUI;
	public:
		Scene(unsigned int width, unsigned int height);
		~Scene();
		bool LoadScene(const char* filePath);
		bool SaveScene(const char* filePath);
		void Render();
	private:
		GameObject* ReadGameObject(std::fstream& file, EntityManager& en, ShaderManager& sm);
	};

}
