#ifndef SHADER_H
#define SHADER_H

#include <SDL_opengles2.h>
#include "SDL.h"
#include "res_path.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader
{
public:
	// the program ID
	unsigned int ID;

	Shader() {}
	// constructor reads and builds the shader
	Shader(std::string vertexFileName, std::string fragmentFileName)
	{
		// load files
		std::string vertexPath = getResourcePath("shaders")+vertexFileName;
		std::string fragmentPath = getResourcePath("shaders")+fragmentFileName;

		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFile;
		std::ifstream fShaderFile;

		// ensure ifstream objects can throw exceptions:
		vShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions (std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			// open files
			vShaderFile.open(vertexPath.c_str());
			fShaderFile.open(fragmentPath.c_str());
			std::stringstream vShaderStream, fShaderStream;
			// read file's buffer contents into streams
			vShaderStream << vShaderFile.rdbuf();
			fShaderStream << fShaderFile.rdbuf();
			// close file handlers
			vShaderFile.close();
			fShaderFile.close();
			// convert stream into string
			vertexCode   = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch(std::ifstream::failure e)
		{
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: couldn't read shader file (%s, %s)", vertexPath.c_str(), fragmentPath.c_str());
		}

		const char* vShaderCode = vertexCode.c_str();
		const char* fShaderCode = fragmentCode.c_str();

		// compile program
		ID = glCreateProgram();
		GLuint vShader = glCreateShader(GL_VERTEX_SHADER);
		GLuint fShader = glCreateShader(GL_FRAGMENT_SHADER);

		glShaderSource(vShader, 1, &vShaderCode, NULL/*terminator*/);
		glShaderSource(fShader, 1, &fShaderCode, NULL/*terminator*/);

		glCompileShader(vShader);
		glCompileShader(fShader);

		int success;
		char infoLog[512];
		glGetShaderiv(vShader, GL_COMPILE_STATUS, &success);
		if( !success )
		{
			glGetShaderInfoLog(vShader, 512, NULL, infoLog);
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: vertex shader compilation failed: %s", infoLog);
			exit(1);
		}
		glGetShaderiv(fShader, GL_COMPILE_STATUS, &success);
		if( !success )
		{
			glGetShaderInfoLog(fShader, 512, NULL, infoLog);
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: fragment shader compilation failed: %s", infoLog);
			exit(1);
		}

		glAttachShader(ID, vShader);
		glAttachShader(ID, fShader);

		glLinkProgram(ID);

		glDeleteShader(vShader);
		glDeleteShader(fShader);

		glGetProgramiv(ID, GL_LINK_STATUS, &success);
		if( !success )
		{
			glGetShaderInfoLog(fShader, 512, NULL, infoLog);
			SDL_LogError(SDL_LOG_CATEGORY_APPLICATION, "error: shader program linking failed: %s", infoLog);
			exit(1);
		}
	}

	// use/activate the shader
	void use()
	{
		glUseProgram(ID);
	}
	// utility uniform functions
	void setBool(const std::string &name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setInt(const std::string &name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const std::string &name, float value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	GLint getUniformLocation(const GLchar* name) const
	{
		return glGetUniformLocation(ID, name);
	}
	GLint getAttribLocation(const GLchar* name) const
	{
		return glGetAttribLocation(ID, name);
	}
};

#endif // SHADER_H
