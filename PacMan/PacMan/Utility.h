#pragma once
#include <string>
#include "Enums.h"
#include "Ghost.h"
#include "Level.h"

using namespace std;

class Utility
{
public:
	//constructors
    Utility();

	//destructors
    ~Utility();

	//methods
    string TransformString(string text, int operation);
    string GhostMode(Ghost& ghost);
    void ReplaceString(string& text, const string from, const char to);
    string Spacer(string format, const int block_width);
};
