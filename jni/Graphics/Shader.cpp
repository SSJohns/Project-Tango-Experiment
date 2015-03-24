// Shader.cpp


#define GL_GLEXT_PROTOTYPES

#include <cstring>
#include <fstream>
#include <stdio.h>
#include <string>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "../IO/FileIO.h"
#include "Shader.h"

using namespace std;


Shader :: Shader(string name) {
	program = glCreateProgram();

	if (program == 0) {
		fprintf(stderr, "Error creating shader program\n");
		exit(1);
	}

	string dir;
	dir = "Resources/Shaders/" + name;
	string vs, fs;

	if (!FileIO::readFileIntoString(dir + "/vertex.sh", vs))
		exit(1);

	if (!FileIO::readFileIntoString(dir + "/frag.sh", fs))
		exit(1);

	addSubShader(vs.c_str(), GL_VERTEX_SHADER);
	addSubShader(fs.c_str(), GL_FRAGMENT_SHADER);

	GLint Success = 0;
	GLchar ErrorLog[1024] = { 0 };

	glLinkProgram(program);
	glGetProgramiv(program, GL_LINK_STATUS, &Success);
	if (Success == 0) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
		exit(1);
	}

	glValidateProgram(program);
	glGetProgramiv(program, GL_VALIDATE_STATUS, &Success);
	if (!Success) {
		glGetProgramInfoLog(program, sizeof(ErrorLog), NULL, ErrorLog);
		fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
		exit(1);
	}
}

GLuint Shader :: getProgram() {
	return program;
}

void Shader :: addSubShader(const char* pShaderText, GLenum ShaderType) {
	GLuint ShaderObj = glCreateShader(ShaderType);

	if (ShaderObj == 0) {
		fprintf(stderr, "Error creating shader type %d\n", ShaderType);
		exit(0);
	}

	const GLchar* p[1];
	p[0] = pShaderText;
	GLint Lengths[1];
	Lengths[0]= strlen(pShaderText);
	glShaderSource(ShaderObj, 1, p, Lengths);
	glCompileShader(ShaderObj);
	GLint success;
	glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success);
	if (!success) {
		GLchar InfoLog[1024];
		glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
		fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
		exit(1);
	}

	glAttachShader(program, ShaderObj);
}
