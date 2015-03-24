// TextureExt.cpp


#include <string>
#include <vector>
#include <GL/freeglut.h>
#include <GL/glut.h>
#include <GL/glext.h>
#include "TextureExt.h"
#include "Texture.h"
using namespace std;


TextureExt :: TextureExt(string fileName) {

	/*if(fileName.find(".gif")) {
		loadMultiframe(fileName);
	}*/
	//else {
		//addFrame(new Texture(fileName));
	//}
}

void TextureExt :: loadMultiframe(string fileName) {
	/*list<Image> imageList; 	

	try {
		readImages(&imageList, fileName);
		
		list<Image> :: const_iterator i;
		for(i = imageList.begin(); i != imageList.end(); i++)
			addFrame(new Texture(*i));
	}
	catch (Magick::Error& Error) {
		
	}	*/
}

void TextureExt :: bind(int ind) {
	getFrame(ind)->bind();
}

void TextureExt :: addFrame(Texture* tex) {
	frameList.push_back(tex);
}

Texture* TextureExt :: getFrame(int ind) {
	return frameList.at(ind % getFrameNumber());
}

/*Texture* TextureExt :: getFrame(float ind) {
	return frameList.at(ind % getFrameNumber());	
}*/

int TextureExt :: getWidth() {
	return frameList.at(0)->getWidth();
}

int TextureExt :: getHeight() {
	return frameList.at(0)->getHeight();
}

int TextureExt :: getFrameNumber() {
	return frameList.size();
}
