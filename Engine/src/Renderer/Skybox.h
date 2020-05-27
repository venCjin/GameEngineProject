#pragma once

#include <string>

namespace sixengine {

	class Skybox
	{
	private:
		unsigned int m_ID;

	public:
		Skybox(std::vector<std::string> faces);
		~Skybox();

		void Load(std::vector<std::string> faces);

		void Bind(unsigned int slot = 0);
		void Unbind();
	};

}


