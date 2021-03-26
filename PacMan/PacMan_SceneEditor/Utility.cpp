#include "Utility.h"
#include <algorithm>
#include <string>

using namespace std;

//constructors
Utility::Utility() {};

//methods
string Utility::TransformString(const string& text, const int operation)
{
    string temp = text;
    switch (operation)
    {
    case 0: // to upper
        transform(temp.begin(), temp.end(), temp.begin(), ::toupper);
        return temp;
    case 1: // to lower
        transform(temp.begin(), temp.end(), temp.begin(), ::tolower);
        return temp;
    }
    return "";
}
void Utility::ReplaceString(string& text, const string from, const char to)
{
    size_t start_pos = 0;
    string to_string = { to };
    while ((start_pos = text.find(from, start_pos)) != std::string::npos) {
        text.replace(start_pos, from.length(), to_string);
        start_pos += to_string.length(); // ...
    }
}
string Utility::Spacer(const string& format, const int block_width)
{
    // used to center the "format" string -> cout the return on either side of the string to print
    int spacer = ((block_width - (int)format.size()) / 2);
    string spaces = "";
    for (int i = 0; i < spacer; i++)
    {
        spaces = spaces + " ";
    }
    return spaces;
}