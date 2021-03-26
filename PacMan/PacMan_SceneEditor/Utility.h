#pragma once
#include <string>

using namespace std;

class Utility
{
public:
	//constructors
    Utility();

	//methods
    static string TransformString(const string& text, const int operation);
    static void ReplaceString(string& text, const string from, const char to);
    static string Spacer(const string& format, const int block_width);
};
