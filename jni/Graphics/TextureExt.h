// TextureExt.h


#ifndef TEXTUREEXT_H
#define TEXTUREEXT_H

#include <string>
#include <vector>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "Texture.h"
using namespace std;

class TextureExt {
	public:
		TextureExt(Texture);
		TextureExt(string);
		void loadMultiframe(string);
		Texture* getFrame(int);
		Texture* getFrame(float);
		void bind(int);
		int getWidth();
		int getHeight();
		int getFrameNumber();
		
	private:
		void addFrame(Texture*);

		vector<Texture*> frameList;
};

#endif
