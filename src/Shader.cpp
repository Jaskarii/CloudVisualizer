#include "Shader.h"
#include <glad/glad.h>
#include <fstream>
#include <string>
#include <sstream>
#include <assert.h>
#include <iostream>
#include "glm/gtc/type_ptr.hpp"

void checkGLErroor()
{
    GLenum err;
    while ((err = glGetError()) != GL_NO_ERROR)
    {
        std::cerr << "OpenGL error in " << std::endl;
    }
}

Shader::Shader(const std::string &filepath)
{
	ShaderProgramSource source = ParseShader(filepath);
	m_RendererId = CreateShader(source.VertexSource, source.GeometrySource, source.FragmentSource);
}

Shader::~Shader()
{
	glDeleteProgram(m_RendererId);
}

void Shader::Bind() const
{
	glUseProgram(m_RendererId);
}

void Shader::UnBind() const
{
	glUseProgram(0);
}

void Shader::SetUniform1i(const std::string &name, int value)
{
	glUniform1i(GetUniformLocation(name), value);
}

void Shader::SetUniform1f(const std::string &name, float value)
{
	glUniform1f(GetUniformLocation(name), value);
}

void Shader::SetUniform2f(const std::string &name, float value1, float value2)
{
	glUniform2f(GetUniformLocation(name), value1, value2);
}

void Shader::SetUniform4f(const std::string &name, float v0, float v1, float v2, float v3)
{
	glUniform4f(GetUniformLocation(name), v0, v1, v2, v3);
}

void Shader::SetUniformMat4f(const std::string &name, const glm::mat4 &matrix)
{
	glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
}

unsigned int Shader::CompileShader(unsigned int type, const std::string &source)
{
	unsigned int id = glCreateShader(type);
	// Return the pointer to the first element of the array
	const char *src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	// Do error handling
	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);

	if (result == GL_FALSE)
	{
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char *message = (char *)alloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		std::cout << message << std::endl;
		glDeleteShader(id);
		return 0;
	}
	return id;
}

int Shader::GetUniformLocation(const std::string &name)
{
	if (m_UniformLocationCache.find(name) != m_UniformLocationCache.end())
	{
		return m_UniformLocationCache[name];
	}
	int location = glGetUniformLocation(m_RendererId, name.c_str());
	if (location == -1)
	{
		std::cout << "Warning: uniform " << name << " doesnt exist" << std::endl;
	}
	m_UniformLocationCache[name] = location;
	return location;
}

ShaderProgramSource Shader::ParseShader(const std::string &file)
{
	std::ifstream stream(file);

	enum class ShaderType
	{
		NONE = -1,
		VERTEX = 0,
		FRAGMENT = 1,
		GEOMETRY = 2
	};

	std::string line;
	std::stringstream ss[3];

	ShaderType type = ShaderType::NONE;
	while (getline(stream, line))
	{
		if (line.find("#shader") != std::string::npos)
		{
			if (line.find("vertex") != std::string::npos)
			{
				type = ShaderType::VERTEX;
			}
			else if (line.find("fragment") != std::string::npos)
			{
				type = ShaderType::FRAGMENT;
			}
			else if (line.find("geometry") != std::string::npos)
			{
				type = ShaderType::GEOMETRY;
			}
		}
		else
		{
			ss[(int)type] << line << '\n';
		}
	}
	return {ss[0].str(), ss[1].str(), ss[2].str()};
}

unsigned int Shader::CreateShader(const std::string &vertexShader, const std::string &geometryShader, const std::string &fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    if (!geometryShader.empty())
    {
        unsigned int gs = CompileShader(GL_GEOMETRY_SHADER, geometryShader);
        glAttachShader(program, gs);
        glDeleteShader(gs);
    }

    glAttachShader(program, vs);
    glAttachShader(program, fs);

    glLinkProgram(program);

    // Check for linking errors
    int linkStatus;
    glGetProgramiv(program, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cout << "Shader linking failed: " << message << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glValidateProgram(program);

    // Check for validation errors
    int validateStatus;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &validateStatus);
    if (validateStatus == GL_FALSE)
    {
        int length;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
        char *message = (char *)alloca(length * sizeof(char));
        glGetProgramInfoLog(program, length, &length, message);
        std::cout << "Shader validation failed: " << message << std::endl;
        glDeleteProgram(program);
        return 0;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    return program;
}

