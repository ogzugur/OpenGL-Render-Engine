#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h> 
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>


class Shader
{
public:
	size_t ID;
	std::string shaderName;
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, std::string shaderName);
	Shader(const GLchar* vertexPath, const GLchar* fragmentPath, const GLchar* geometryPath, std::string shaderName);
	void use();
	void setBool(const std::string &name, bool value) const;
	void setInt(const std::string &name, int value) const;
	void setFloat(const std::string &name, float value) const;
private:
	void checkCompileErrors(unsigned int shader, std::string type);
};

#endif