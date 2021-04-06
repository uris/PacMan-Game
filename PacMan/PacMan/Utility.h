#pragma once
#include <string>
#include <iostream>

using namespace std;

class Utility
{
public:
    //constructors
    Utility();

    //methods
    static string TransformString(const string& text, const int operation);
    static void ReplaceString(string& text, const string from, const char to);
    static void ReplaceString(string& text, const char from, const char to);
    static string Spacer(const string& format, const int block_width);
    static string GetMenuFromFile(const string& filename);
    static string GetTemplateFromFile(const string& file_name);

    template <typename Type, bool number = true>
    static Type ProcessOption(string label);
    
};