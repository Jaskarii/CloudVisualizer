#pragma once
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct ShaderProgramSource
{
	std::string VertexSource;
	std::string FragmentSource;
	std::string GeometrySource;
};

class Shader
{
private:
	std::string m_FilePath;
	unsigned int m_RendererId;
	std::unordered_map<std::string, int> m_UniformLocationCache;

public:
	Shader(const std::string &filepath);

	~Shader();

	void Bind() const;
	void UnBind() const;

	void SetUniform1i(const std::string &name, int value);
	void SetUniform1f(const std::string &name, float value);
	void SetUniform2f(const std::string &name, float value1, float value2);
	void SetUniform4f(const std::string &name, float v0, float v1, float f2, float f3);
	void SetUniformMat4f(const std::string &name, const glm::mat4 &matrix);

private:
	unsigned int CompileShader(unsigned int type, const std::string &source);
	int GetUniformLocation(const std::string &name);
	ShaderProgramSource ParseShader(const std::string &file);
	unsigned int CreateShader(const std::string &vertexShader, const std::string &geometryShader, const std::string &fragmentShader);
};
