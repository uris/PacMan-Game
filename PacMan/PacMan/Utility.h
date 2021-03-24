#pragma once
#include <string>

using namespace std;

class Utility
{
public:
	//constructors
    Utility();

	//methods
    string TransformString(const string& text, const int operation);
    void ReplaceString(string& text, const string from, const char to);
    string Spacer(const string& format, const int block_width);
};
