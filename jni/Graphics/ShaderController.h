// ShaderController.h


#ifndef SHADERCONTROLLER_H
#define SHADERCONTROLLER_H

#include <string>
#include <map>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "Shader.h"

using namespace std;

class ShaderController {
	public:
		ShaderController();
		Shader* getShader(string);
		GLuint getProgram(string);

	private:
		void initialize();
		void addShader(string);
		map<string, Shader*> shaderMap;
};

#endif
