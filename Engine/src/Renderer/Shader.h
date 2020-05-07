#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
namespace sixengine {

	class Shader
	{
	private:
		unsigned int m_ID;

	private:
		void CompileAndAttach(unsigned int shaderType, const char* shaderCode);
		void LinkProgram();

	public:
		Shader(const char* shaderPath);
		~Shader();
		
		void Bind() const;
		void Unbind() const;

		inline unsigned int GetID() const { return m_ID; }

		void SetInt(const std::string &name, int value) const;
		void SetFloat(const std::string &name, float value) const;
		void SetBool(const std::string &name, bool value) const;
		void SetColor(const std::string &name, float color[]) const;
		void SetVec3(const std::string &name, const glm::vec3 &vec) const;
		void SetMat4(const std::string &name, const glm::mat4 &mat) const;
	};

}