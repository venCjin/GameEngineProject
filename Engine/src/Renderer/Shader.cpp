#include "pch.h"
#include "Shader.h"

#include <glad/glad.h>

namespace sixengine {	

	Shader::Shader(const char * vertexPath, const char * fragmentPath)
	{
		std::string vertexCode;
		std::string fragmentCode;

		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);

			std::stringstream vShaderStream, fShaderStream;
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();

			vShaderFile.close();
			fShaderFile.close();

			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();			
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		unsigned int vertex = Compile(GL_VERTEX_SHADER, vShaderCode);
		unsigned int fragment = Compile(GL_FRAGMENT_SHADER, fShaderCode);

		LinkProgram(vertex, fragment);

		glDeleteShader(vertex);
		glDeleteShader(fragment);

	}

	unsigned int Shader::Compile(unsigned int shaderType, const char * shaderCode)
	{
		unsigned int shader = glCreateShader(shaderType);;
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);

		int success;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLint length;
			glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);

			std::vector<GLchar> message(length);
			glGetProgramInfoLog(m_ID, length, &length, &message[0]);
			std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n" << message.data() << std::endl;
		};
		return shader;
	}

	void Shader::LinkProgram(unsigned int vertex, unsigned int fragment)
	{
		m_ID = glCreateProgram();

		glAttachShader(m_ID, vertex);
		glAttachShader(m_ID, fragment);
		glLinkProgram(m_ID);

		//glValidateProgram(m_ID) ?

		GLint success = 0;

		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLint length;
			glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);

			std::vector<GLchar> message(length);
			glGetProgramInfoLog(m_ID, length, &length, &message[0]);
			std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << message.data() << std::endl;
		}
	}

	void Shader::Bind() const
	{
		glUseProgram(m_ID);
	}

	void Shader::Unbind() const
	{
		glUseProgram(0);
	}

	void Shader::SetInt(const std::string & name, int value) const
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void Shader::SetFloat(const std::string & name, float value) const
	{
		glUniform1f(glGetUniformLocation(m_ID, name.c_str()), value);
	}

	void Shader::SetBool(const std::string & name, bool value) const
	{
		glUniform1i(glGetUniformLocation(m_ID, name.c_str()), (int)value);
	}

	void Shader::SetColor(const std::string & name, float color[]) const
	{
		glUniform3f(glGetUniformLocation(m_ID, name.c_str()), color[0], color[1], color[2]);
	}

	void Shader::SetVec3(const std::string & name, const glm::vec3 & vec) const
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vec[0]);
	}

	void Shader::SetMat4(const std::string & name, const glm::mat4 & mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

	/*void Shader::SetBoneTransform(const std::string & name, const Matrix4f & Transform)
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_TRUE, (const GLfloat*)Transform);
	}*/


	Shader::~Shader()
	{
	}

}