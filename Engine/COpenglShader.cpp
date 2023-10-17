#include "irrString.h"
#include "IReadFile.h"
#include "IOpenGlShader.h"

#include "COpenGLCommon.h"
#include "COpenglShader.h"



namespace irr
{
namespace video
{
COpenglShader::COpenglShader(io::IReadFile* vs, io::IReadFile* fs)
{
	// 1. retrieve the vertex/fragment source code from filePath
	core::stringc vertexCode;
	core::stringc fragmentCode;

	readstringc(vs, vertexCode);
	readstringc(fs, fragmentCode);

	const c8* v = vertexCode.c_str();
	const c8* f = fragmentCode.c_str();

	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &v, NULL);
	glCompileShader(vertex);
	checkCompileErrors(vertex, "VERTEX");
	// fragment Shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &f, NULL);
	glCompileShader(fragment);
	checkCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	checkCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

COpenglShader::~COpenglShader()
{

}

void COpenglShader::use()
{
	glUseProgram(ID);
}

void COpenglShader::setBool(const core::stringc& name, bool value)const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}

void COpenglShader::setInt(const core::stringc& name, s32 value)const
{
	glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}

void COpenglShader::setFloat(const core::stringc& name, f32 value)const
{
	glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}

void COpenglShader::setMat4(const core::stringc& name, const core::matrix4& mat) const
{
	glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0]);
}

void COpenglShader::checkCompileErrors(u32 shader, const core::stringc& type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			//std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			//std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}

void COpenglShader::readstringc(io::IReadFile* file, core::stringc& shader)
{
	c8 c;

	shader = "";
	file->read(&c, 1);
	while (c)
	{
		shader.append(c);
		if (!file->read(&c, 1))
			break;
	}
	//// read extra 0 upon odd file position
	//if (File->getPos() & 0x1)
	//{
	//	File->read(&c, 1);
	//	return (name.size() + 2);
	//}
}

}
}