#ifndef SHADER_H
#define SHADER_H

#pragma once
#include <glm/glm.hpp>

#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include "gl/glew.h"
class myShader
{
public:
	//程序ID
	unsigned int ID;
	myShader() {}
	myShader(const GLchar* vertexPath, const GLchar* fragmentPath)
	{
		//！1、读取着色器的代码
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;
		//确保文件流会输出异常
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

		try
		{
			//打开文件
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream, fShaderStream;
			//读取文件到流中
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			//关闭文件
			vShaderFile.close();
			fShaderFile.close();
			//将流转换为字符串
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			std::cout << "错误：读取文件失败，请检查文件是否存在！" << std::endl;
		}

		//！2、编译着色器
		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();
		unsigned int vertex, fragment;
		int success;
		char infoLog[512];

		//顶点着色器
		vertex = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex, 1, &vShaderCode, NULL);
		glCompileShader(vertex);
		glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(vertex, 512, NULL, infoLog);
			std::cout << "编译顶点着色器失败，错误信息：" << infoLog << std::endl;
		}

		//片元着色器
		fragment = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment, 1, &fShaderCode, NULL);
		glCompileShader(fragment);
		glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
		if(!success)
		{
			glGetShaderInfoLog(fragment, 512, NULL, infoLog);
			std::cout << "编译片元着色器失败，错误信息：" << infoLog << std::endl;
			//USES_CONVERSION;
			//std::wstring abc = std::wstring(_T("编译片元着色器失败，错误信息：")) + A2W(infoLog) + A2W("\n");
			//OutputDebugString(abc.c_str());
		}

		//着色器程序
		ID = glCreateProgram();
		glAttachShader(ID, vertex);
		glAttachShader(ID, fragment);
		glLinkProgram(ID);
		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if(!success)
		{
			glGetProgramInfoLog(ID, 512, NULL, infoLog);
			std::cout << "连接着色器程序失败，错误信息：" << infoLog << std::endl;
		}

		//删除着色器程序
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	inline bool Compile(const GLchar* vertexPath, const GLchar* fragmentPath);
	//使用着色器
	inline void use();
	//设置uniform变量
	inline void setBool(const std::string& name, bool value) const;
	inline void setInt(const std::string& name, int value) const;
	inline void setFloat(const std::string& name, float value) const;
	inline void setMat4(const std::string& name, float value[]) const;
	inline void setMat4(const std::string& name, const float* value) const;
	inline void setMat4(const std::string& name, const glm::mat4& value) const;
	inline void setVec3(const std::string& name, const glm::vec3& value) const;
	inline void setVec3(const std::string& name, float x, float y, float z) const;
	inline void setVec3(const std::string& name, float value[])const;
	inline void setVec4(const std::string& name, float x, float y, float z, float w) const;
	inline void setVec2(const std::string& name, float x, float y)const;
	inline bool uniformMatrix3f(const char* varName, float* mat, bool transpose = false);
	inline bool uniformMatrix4f(const char* varName, float* mat, bool transpose = false);
	inline bool uniform1i(const char* varName, int value);
	inline bool uniform1f(const char* varName, float value);
	inline bool uniform2f(const char* varName, float x, float y);
	inline bool uniform3f(const char* varName, float x, float y, float z);
	inline bool uniform4f(const char* varName, float x, float y, float z, float w);
	inline bool uniform1fv(const char* varName, GLuint count, float* values);
	inline bool uniform2fv(const char* varName, float* values);
	inline bool uniform3fv(const char* varName, float* values);
	inline bool uniform4fv(const char* varName, float* values);
};

#ifdef _WIN32
#include <tchar.h>
#include <windows.h>
#include <atlbase.h>
#endif

//myShader::myShader(const GLchar* vertexPath, const GLchar* fragmentPath) {
//	//！1、读取着色器的代码
//	std::string vertexCode;
//	std::string fragmentCode;
//	std::ifstream vShaderFile;
//	std::ifstream fShaderFile;
//	//确保文件流会输出异常
//	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
//
//	try {
//		//打开文件
//		vShaderFile.open(vertexPath);
//		fShaderFile.open(fragmentPath);
//		std::stringstream vShaderStream, fShaderStream;
//		//读取文件到流中
//		vShaderStream << vShaderFile.rdbuf();
//		fShaderStream << fShaderFile.rdbuf();
//		//关闭文件
//		vShaderFile.close();
//		fShaderFile.close();
//		//将流转换为字符串
//		vertexCode = vShaderStream.str();
//		fragmentCode = fShaderStream.str();
//	}
//	catch (std::ifstream::failure e) {
//		std::cout << "错误：读取文件失败，请检查文件是否存在！" << std::endl;
//	}
//
//	//！2、编译着色器
//	const char* vShaderCode = vertexCode.c_str();
//	const char* fShaderCode = fragmentCode.c_str();
//	unsigned int vertex, fragment;
//	int success;
//	char infoLog[512];
//
//	//顶点着色器
//	vertex = glCreateShader(GL_VERTEX_SHADER);
//	glShaderSource(vertex, 1, &vShaderCode, NULL);
//	glCompileShader(vertex);
//	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
//		std::cout << "编译顶点着色器失败，错误信息：" << infoLog << std::endl;
//	}
//
//	//片元着色器
//	fragment = glCreateShader(GL_FRAGMENT_SHADER);
//	glShaderSource(fragment, 1, &fShaderCode, NULL);
//	glCompileShader(fragment);
//	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
//	if (!success) {
//		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
//		std::cout << "编译片元着色器失败，错误信息：" << infoLog << std::endl;
//		USES_CONVERSION;
//		//std::wstring abc = std::wstring(_T("编译片元着色器失败，错误信息：")) + A2W(infoLog) + A2W("\n");
//		//OutputDebugString(abc.c_str());
//	}
//
//	//着色器程序
//	ID = glCreateProgram();
//	glAttachShader(ID, vertex);
//	glAttachShader(ID, fragment);
//	glLinkProgram(ID);
//	glGetProgramiv(ID, GL_LINK_STATUS, &success);
//	if (!success) {
//		glGetProgramInfoLog(ID, 512, NULL, infoLog);
//		std::cout << "连接着色器程序失败，错误信息：" << infoLog << std::endl;
//	}
//
//	//删除着色器程序
//	glDeleteShader(vertex);
//	glDeleteShader(fragment);
//}
bool myShader::Compile(const GLchar* vertexPath, const GLchar* fragmentPath)
{
	//！1、读取着色器的代码
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	//确保文件流会输出异常
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	try
	{
		//打开文件
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		//读取文件到流中
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		//关闭文件
		vShaderFile.close();
		fShaderFile.close();
		//将流转换为字符串
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch(std::ifstream::failure e)
	{
		std::cout << "错误：读取文件失败，请检查文件是否存在！" << std::endl;
	}

	//！2、编译着色器
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	unsigned int vertex, fragment;
	int success;
	char infoLog[512];

	//顶点着色器
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(vertex, 512, NULL, infoLog);
		std::cout << "编译顶点着色器失败，错误信息：" << infoLog << std::endl;
	}

	//片元着色器
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if(!success)
	{
		glGetShaderInfoLog(fragment, 512, NULL, infoLog);
		std::cout << "编译片元着色器失败，错误信息：" << infoLog << std::endl;
		USES_CONVERSION;
		//std::wstring abc = std::wstring(_T("编译片元着色器失败，错误信息：")) + A2W(infoLog) + A2W("\n");
		//OutputDebugString(abc.c_str());
	}

	//着色器程序
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	glGetProgramiv(ID, GL_LINK_STATUS, &success);
	if(!success)
	{
		glGetProgramInfoLog(ID, 512, NULL, infoLog);
		std::cout << "连接着色器程序失败，错误信息：" << infoLog << std::endl;
		return false;
	}

	//删除着色器程序
	glDeleteShader(vertex);
	glDeleteShader(fragment);
	return true;
}
///使用着色器
void myShader::use()
{
	glUseProgram(ID);
}

/// 设置变量值
void myShader::setBool(const std::string& name, bool value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
void myShader::setInt(const std::string& name, int value) const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
void myShader::setFloat(const std::string& name, float value) const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void myShader::setMat4(const std::string& name, float value[]) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void myShader::setMat4(const std::string& name, const float* value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, value);
}

void myShader::setMat4(const std::string& name, const glm::mat4& value) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &(value[0].x));
}

void myShader::setVec3(const std::string& name, const glm::vec3& value) const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
}
void myShader::setVec3(const std::string& name, float x, float y, float z) const
{
	glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
}

void myShader::setVec3(const std::string& name, float value[])const
{
	glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, value);
}

void myShader::setVec4(const std::string& name, float x, float y, float z, float w) const
{
	glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);

}



inline bool myShader::uniform1i(const char* varName, int value)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform1i(i, value);
	return true;
}


inline bool myShader::uniform1f(const char* varName, float value)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform1f(i, value);
	return true;
}

inline bool myShader::uniform2f(const char* varName, float x, float y)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform2f(i, x, y);
	return true;
}

inline bool myShader::uniform3f(const char* varName, float x, float y, float z)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform3f(i, x, y, z);
	return true;
}

inline bool myShader::uniform4f(const char* varName, float x, float y, float z, float w)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform4f(i, x, y, z, w);
	return true;
}

inline bool myShader::uniform1fv(const char* varName, GLuint count, float* values)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform1fv(i, count, values);
	return true;
}

inline bool myShader::uniform2fv(const char* varName, float* values)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform2fv(i, 1, values);
	return true;
}

inline bool myShader::uniform3fv(const char* varName, float* values)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform3fv(i, 1, values);
	return true;
}

inline bool myShader::uniform4fv(const char* varName, float* values)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniform4fv(i, 1, values);
	return true;
}

inline bool myShader::uniformMatrix3f(const char* varName, float* mat, bool transpose)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniformMatrix3fv(i, 1, transpose, mat);
	return true;
}

inline bool myShader::uniformMatrix4f(const char* varName, float* mat, bool transpose)
{
	GLint i = glGetUniformLocation(ID, varName);
	if(i == -1) return false;

	glUniformMatrix4fv(i, 1, transpose, mat);
	return true;
}

#endif