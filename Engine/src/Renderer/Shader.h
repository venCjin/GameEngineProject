#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <glad/glad.h>
#include <map>

namespace sixengine {

	class Shader
	{
	private:
		unsigned int m_ID;

	private:
		void CompileAndAttach(GLenum shaderType, const char* shaderCode);
		void LinkProgram();
		std::map<GLenum, std::string> m_shaderTypeMapping{
			{GL_VERTEX_SHADER, "GL_VERTEX_SHADER"},
			{GL_TESS_CONTROL_SHADER, "GL_TESS_CONTROL_SHADER"},
			{GL_TESS_EVALUATION_SHADER, "GL_TESS_EVALUATION_SHADER"},
			{GL_GEOMETRY_SHADER, "GL_GEOMETRY_SHADER"},
			{GL_FRAGMENT_SHADER, "GL_FRAGMENT_SHADER"},
			{GL_COMPUTE_SHADER, "GL_COMPUTE_SHADER"}
		};

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
		void SetVec4(const std::string& name, const glm::vec4& vec) const;
		void SetMat4(const std::string &name, const glm::mat4 &mat) const;
	};

}