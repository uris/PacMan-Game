#include "Draw.h"
#include "Utility.h"
#include "MainMenu.h"
#include <conio.h>
#include <iostream> // base input / output
#include <Windows.h> // used to output colors

using namespace std;

//constructors
Draw::Draw() {}

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
string Draw::WriteEmptyLine(const int length)
{
    string empty_line = "";
    for (int i = 0; i < length; i++)
    {
        empty_line += " ";
    }
    return (empty_line);
}
string Draw::WriteEmptyLine(int height, int length)
{
    string empty_line = "";
    for (int h = 0; h < height; h++)
    {
        for (int i = 0; i < length; i++)
        {
            empty_line += " ";
        }
        empty_line += "\n";
    }
    return (empty_line);
}
string Draw::Credits()
{
  
    Draw::SetConsoleFont(true, Resolution::HUGE);
    string ghost;
    Draw::ShowConsoleCursor(false);

    ghost += "               *********\n";
    ghost += "           *****************\n";
    ghost += "         *********************\n";
    ghost += "       ******####********####**\n";
    ghost += "       ****#########***#########\n";
    ghost += "       ****#####    ***#####    *\n";
    ghost += "    *******#####    ***#####    ***\n";
    ghost += "    *********#####********####*****\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *****  *******    ******  *****\n";
    ghost += "    ***      *****    ****      ***\n";

    // print image
    string format = Utility::Spacer("PACMAN 2021", 40);
    cout << endl << endl;
    Draw::SetColor(7);
    cout << ghost;
    Draw::SetColor(7);
    cout << endl;
    cout << format;
    cout << "PACMAN 2021";
    cout << format;
    cout << endl << endl;

    // play intro
    PlaySound(TEXT("sfx_intro.wav"), NULL, SND_FILENAME | SND_SYNC);

    string options[3][2];
    options[0][0] = "#play";
    options[0][1] = "Play Game";
    options[1][0] = "#edit";
    options[1][1] = "Edit Scenes";
    options[2][0] = "#quit";
    options[2][1] = "Quit";

    MainMenu choose;
    choose.Template(MenuTemplates::CHOOSE_EDIT_PLAY);
    choose.Create(options, 3);
    return choose.Show();

}
bool Draw::SetConsoleFont(const bool pacman_font, const Resolution resolution)
{
    int result;


    CONSOLE_FONT_INFOEX info = { 0 };
    info.cbSize = sizeof(info);
    info.dwFontSize.Y = (int)resolution; // leave X as zero
    info.FontWeight = FW_NORMAL;
    if (!pacman_font)
        wcscpy_s(info.FaceName, L"Consolas");
    else
        wcscpy_s(info.FaceName, L"PacMan Console");

    result = SetCurrentConsoleFontEx(GetStdHandle(STD_OUTPUT_HANDLE), NULL, &info);
    
    if (result)
        Draw::SetConsoleSize(resolution);

    return true;
}
void Draw::SetConsoleSize(const Resolution resolution)
{
    Coord console_size;
  
    switch (resolution)
    {
    case Resolution::TINY:
        console_size = {410, 400};
        break;
    case Resolution::SMALL:
        console_size = { 598, 510 };
        break;
    case Resolution::DEFAULT:
    case Resolution::NORMAL:
        console_size = { 692, 650 };
        break;
    case Resolution::LARGE:
        console_size = { 880, 760 };
        break;
    case Resolution::EXTRA_LARGE:
        console_size = { 974, 860 };
        break;
    case Resolution::HUGE:
        console_size = { 1256, 1100 };
        break;
    }

    HWND console = GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect);
    MoveWindow(console, 50, 50, console_size.row, console_size.col, TRUE);
}