// Image.cpp


#define cimg_use_png

#include <iostream>
#include "Image.h"
#include <string>
#include "CImg.h"
#include "../Global.h"
using namespace cimg_library;
using namespace std;


Image :: Image(string fileName) {

	image = CImg<>(fileName.c_str()).normalize(0,255);

	width = image.width();
	height = image.height();
}

int Image :: getWidth() {
	return width;
}

int Image :: getHeight() {
	return height;
}

void Image :: interleave(unsigned char* out) {
	unsigned char* data = image.data();

	int i, numChannels;
	i = 0;
	numChannels = image.spectrum();

	if(numChannels == 1) {
		for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++) {

				for(int c = 0; c < 3; c++) {
					out[i+c] = image(x,y,0);
				}
				out[i+3] = 255;

				i += 4;
			}
	}
	else if(numChannels == 2) {
		for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++) {

				for(int c = 0; c < 3; c++) {
					out[i+c] = image(x,y,0);
				}
				out[i+3] = image(x,y,1);

				i += 4;
			}
	}
	else if(numChannels == 3) {
		for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++) {

				for(int c = 0; c < 3; c++) {
					out[i+c] = image(x,y,c);
				}
				out[i+3] = 255;

				i += 4;
			}
	}
	else if(numChannels == 4) {
		for(int y = 0; y < height; y++)
			for(int x = 0; x < width; x++) {

				for(int c = 0; c < 4; c++) {
					out[i+c] = image(x,y,c);
				}

				i += 4;
			}
	}
}

void* Image :: getData() {
	return image.data();
}

int Image :: getValue(int x, int y) {
	if(image.spectrum() == 1)
		return image(x,y,0);
	else
		return .21*image(x,y,0) + .72*image(x,y,1) + .07*image(x,y,2);
}

void Image :: getPixelData() {
	//R = image(x,y,0)
	//G = image(x,y,1)
	//B = image(x,y,2)
}
