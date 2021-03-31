#pragma once
#include <string>

using namespace std;

class Draw
{
public:
    //constructors
    Draw();

    //methods
    static void SetColor(const int color);
    static void ShowColors(const int colors);
    static void CursorTopLeft(const int rows);
    static void ShowConsoleCursor(const bool showFlag);
    static string WriteEmptyLine(const int length);
    static string Credits();
};
