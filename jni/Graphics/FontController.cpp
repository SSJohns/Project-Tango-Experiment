// FontController.cpp


#include <iostream>
#include <map>
#include <string>
#include "Font.h"
#include "FontController.h"
#include <android/log.h>
using namespace std;

FontController :: FontController() {
	initialize();
}

Font2D* FontController :: getFont(string name) {
	return fontMap[name];
}		

void FontController :: initialize() {
	addFont("8bit",false);
}

void FontController :: addFont(string name, bool hasCaps) {
	fontMap[name] = new Font2D(name, hasCaps);
}
