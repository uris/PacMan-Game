 #include "Draw.h"
#include <iostream> // base input / output
#include <Windows.h> // used to output colors

using namespace std;

//constructors
Draw::Draw(){}

//methods
void Draw::SetColor(const int color)
{
    // windows only - sets text color for command line
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color);
}
void Draw::ShowColors(const int colors)
{
    for (int i = 0; i < colors; i++)
    {
        SetColor(i);
        cout << "This is color " << i << endl;
    }
}
void Draw::CursorTopLeft(const int rows)
{
    for (int i = 0; i < rows; i++) // up one line
    {
        cout << "\x1b[A";
    }
    cout << "\r";
}
void Draw::ShowConsoleCursor(const bool showFlag)
{
    HANDLE out = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_CURSOR_INFO     cursorInfo;

    GetConsoleCursorInfo(out, &cursorInfo);
    cursorInfo.bVisible = showFlag; // set the cursor visibility
    SetConsoleCursorInfo(out, &cursorInfo);
}