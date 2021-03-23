#pragma once
#include <string>

using namespace std;

class Utility
{
public:
	//constructors
    Utility();

	//methods
    string TransformString(string text, int operation);
    void ReplaceString(string& text, string from, char to);
    string Spacer(string format, int block_width);
};
