#include "pch.h"
#include "Shader.h"


namespace sixengine {

	Shader::~Shader()
	{
	}

	Shader::Shader(const char * shaderPath)
	{
		std::stringstream code[6];
		int shader = -1;

		std::ifstream shaderFile;
		//shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try {
			shaderFile.open(shaderPath);

			std::string line;
			while (getline(shaderFile, line))
			{
				if (line.find("#shader") != std::string::npos)
				{
					if (line.find("vertex") != std::string::npos)
						shader = 0;
					else if (line.find("tesselationControl") != std::string::npos)
						shader = 1;
					else if (line.find("tesselationEvaluation") != std::string::npos)
						shader = 2;
					else if (line.find("geometry") != std::string::npos)
						shader = 3;
					else if (line.find("fragment") != std::string::npos)
						shader = 4;
					else if (line.find("compute") != std::string::npos)
						shader = 5;
				}
				else
					if (shader != -1) code[shader] << line << "\n";
			}

			shaderFile.close();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
		}

		m_ID = glCreateProgram();

		if (code[0].str().length() > 0) CompileAndAttach(GL_VERTEX_SHADER, code[0].str().c_str());
		if (code[1].str().length() > 0) CompileAndAttach(GL_TESS_CONTROL_SHADER, code[1].str().c_str());
		if (code[2].str().length() > 0) CompileAndAttach(GL_TESS_EVALUATION_SHADER, code[2].str().c_str());
		if (code[3].str().length() > 0) CompileAndAttach(GL_GEOMETRY_SHADER, code[3].str().c_str());
		if (code[4].str().length() > 0) CompileAndAttach(GL_FRAGMENT_SHADER, code[4].str().c_str());
		if (code[5].str().length() > 0) CompileAndAttach(GL_COMPUTE_SHADER, code[5].str().c_str());

		LinkProgram();
	}

	void Shader::CompileAndAttach(GLenum shaderType, const char * shaderCode)
	{
		unsigned int shader = glCreateShader(shaderType);;
		glShaderSource(shader, 1, &shaderCode, NULL);
		glCompileShader(shader);

		int success;

		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			GLint length = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

			length = (length <= 0) ? 256 : length;

			std::vector<GLchar> message(length);
			glGetShaderInfoLog(shader, length, &length, &message[0]);
			std::string errorMessage = std::string("ERROR::SHADER::") + m_shaderTypeMapping[shaderType] + std::string("::COMPILATION_FAILED\n") + std::string(message.data());
			LOG_ERROR(errorMessage);
		}
		else
		{
			glAttachShader(m_ID, shader);
			glDeleteShader(shader);
		}
	}

	void Shader::LinkProgram()
	{
		glLinkProgram(m_ID);

		GLint success = 0;

		glGetProgramiv(m_ID, GL_LINK_STATUS, &success);
		if (!success)
		{
			GLint length;
			glGetProgramiv(m_ID, GL_INFO_LOG_LENGTH, &length);

			length = (length <= 0) ? 256 : length;

			std::vector<GLchar> message(length);
			glGetProgramInfoLog(m_ID, length, &length, &message[0]);
			std::string errorMessage = std::string("ERROR::SHADER::PROGRAM::LINKING_FAILED\n") + std::string(message.data());
			LOG_ERROR(errorMessage);
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

	void Shader::SetVec4(const std::string& name, const glm::vec4 & vec) const
	{
		glUniform3fv(glGetUniformLocation(m_ID, name.c_str()), 1, &vec[0]);
	}

	void Shader::SetMat4(const std::string & name, const glm::mat4 & mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(m_ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}

}