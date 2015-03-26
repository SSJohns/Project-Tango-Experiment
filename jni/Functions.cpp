// Functions.cpp

#include "Functions.h"
#include <string>
#include <sstream>
using namespace std;

string to_string(float number) {
    ostringstream buff;
    buff << number;
    return buff.str();
}
