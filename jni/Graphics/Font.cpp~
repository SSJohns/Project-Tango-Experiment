// Font.cpp


#include <iostream>
#include <cctype>
#include <string>
#include <map>
#include "Texture.h"
#include "Font.h"
#include "../Global.h"
using namespace std;


Font2D :: Font2D(string fontName, bool ca) {
	isCaseEnabled = ca;

	fontDir = "Resources/Fonts/" + fontName + "/";
	char c;

	if(!isCaseEnabled) {
		for(int i = 0; i < 26; i++)
			addChar('a' + i);
		for(int i = 0; i < 10; i++)
			addChar('0' + i);

		addChar(',');
		addChar('!');
		addChar('.');
		addChar(';');
		addChar(':');
		addChar('-');
	}
}

Texture* Font2D :: getChar(char c) {
	if(isalpha(c)) {
		if(!isCaseEnabled)
			return fontMap[tolower(c)];
		else
			return fontMap[c];
	}
	else// if(isdigit(c))
		return fontMap[c];

	//return NULL;
}

void Font2D :: addChar(char c) {
	addChar(c, (fontDir + c) + ".png");
}

void Font2D :: addChar(char c, string fileName) {
	Texture* t = new Texture(fileName, true);

	fontMap[c] = t;
}
