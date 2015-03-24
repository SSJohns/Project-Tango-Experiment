// Texture.h


#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include "../tango_data.h"
#include "Image.h"
using namespace std;

class Texture {
	public:
		Texture(const string&, bool);
		
		int getWidth();
		int getHeight();

		void bind();
		void bind(GLenum);
		
	private:

		static int texNum;

		string fileName;

		GLuint m_textureObj;
		Image* image;
};

#endif
