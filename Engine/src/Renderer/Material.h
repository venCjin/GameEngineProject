#pragma once
#include "Shader.h"

namespace sixengine {

	class Material
	{
	private:
		Shader* m_Shader;
		glm::vec4 m_Texture;
		std::string m_Name;

	public:
		Material(Shader* shader, glm::vec4 textureID, std::string name = "noname");
		~Material();

		void SetShader(Shader* shader);
		void SetTexture(glm::vec4 textureID);
		void SetName(const std::string& name);

		inline Shader* GetShader() const { return m_Shader; }
		inline glm::vec4 GetTexture() const { return m_Texture; }
		inline std::string GetName() const { return m_Name; }
	};

}