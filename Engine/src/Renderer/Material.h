#pragma once
#include "Shader.h"

namespace sixengine {

	class Material
	{
	private:
		Shader *m_Shader;
		std::string m_Name;

	public:

		Material();

		void Bind();
		void Unbind();
		
		void SetShader(Shader &shader);
		//void SetTexture();
		void SetColor(const std::string &name, float color[]);

		void SetName(std::string name);
		std::string GetName();

		~Material();
	};

}