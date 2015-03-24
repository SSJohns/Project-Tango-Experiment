// Texture.cpp


#include <iostream>
#include "../tango_data.h"
#include "Texture.h"
#include "Image.h"
using namespace std;

int Texture :: texNum = 0;



Texture::Texture(const string& fileName, bool isFont) {

	image = new Image(fileName);

	glGenTextures(1, &m_textureObj);
	glBindTexture(GL_TEXTURE_2D, m_textureObj);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0);
	//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);

	if(isFont) {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}
	else {
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR); 
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	}
		
		/*glPixelStoref(GL_PACK_ROW_LENGTH,1);
		glPixelStoref(GL_PACK_SKIP_PIXELS,1);
		glPixelStoref(GL_PACK_SKIP_ROWS,1);
		glPixelStoref(GL_PACK_ALIGNMENT,1);*/


	unsigned char* data = new unsigned char[getWidth()*getHeight()*4];
	image->interleave(data);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, getWidth(), getHeight(), 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glBindTexture(GL_TEXTURE_2D, 0);
}


int Texture :: getWidth() {
	return image->getWidth();
}

int Texture :: getHeight() {
	return image->getHeight();
}


void Texture::bind() {
	bind(GL_TEXTURE0);
}

void Texture::bind(GLenum TextureUnit) {
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_2D, m_textureObj);
}

