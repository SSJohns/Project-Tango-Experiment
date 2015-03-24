// Shader.h


#ifndef SHADER_H
#define SHADER_H

#include <string>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
using namespace std;

class Shader {
	public:
		Shader(string);
		GLuint getProgram();

	private: 
		void addSubShader(const char*, GLenum);
		GLuint program;
};

#endif
