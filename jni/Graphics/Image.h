// Image.h


#ifndef IMAGE_H
#define IMAGE_H

#define cimg_display 0
#define cimg_use_png

#include <string>
#include "../CImg.h"
using namespace cimg_library;
using namespace std;

class Image {
	public:
		Image(string);
		int getWidth();
		int getHeight();
		void* getData();
		int getValue(int, int);
		void getPixelData();		
		void interleave(unsigned char*);

	private:


		CImg<unsigned char> image;
		//CImg<unsigned char> iImg;

		int width;
		int height;
};

#endif


