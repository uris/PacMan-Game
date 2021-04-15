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
void Draw::SetConsoleSize(const Resolution resolution, const int rows, const int cols)
{
    HWND console = GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect);
    
    Coord console_size((int)(ConsoleRect.right - ConsoleRect.left), (int)(ConsoleRect.bottom - ConsoleRect.top));
    Coord console_resize;

    const float col = 47.0f;
    const float row = 29.0f;

    switch (resolution)
    {
    case Resolution::TINY:
        console_resize = { (int)(cols * (598.0f / col)), (int)(rows * (388.0f / row)) };
        break;
    case Resolution::SMALL:
        console_resize = { (int)(cols * (786.0f / col)), (int)(rows * (504.0f / row)) };
        break;
    case Resolution::NORMAL:
        console_resize = { (int)(cols * (974.0f / col)), (int)(rows * (620.0f / row)) };
        break;
    case Resolution::LARGE:
        console_resize = { (int)(cols * (1209.0f / col)), (int)(rows * (736.0f / row)) };
        break;
    case Resolution::EXTRA_LARGE:
        console_resize = { (int)(cols * (1350.0f / col)), (int)(rows * (852.0f / row)) };
        break;
    case Resolution::HUGE:
        console_resize = { (int)(cols * (1726.0f / col)), (int)(rows * (1084.0f / row)) };
        break;
    default:
        console_resize = { (int)(cols * (974.0f / col)), (int)(rows * (620.0f / row)) };
        break;
    }

    if (console_size != console_resize)
    {
        MoveWindow(console, ConsoleRect.left, ConsoleRect.top, console_resize.row, console_resize.col, TRUE);
        SetWindowLong(console, GWL_STYLE, GetWindowLong(console, GWL_STYLE) & ~WS_MAXIMIZEBOX & ~WS_SIZEBOX);
        system("cls");
    }
}
Coord Draw::GetConsoleSize()
{
    HWND console = GetConsoleWindow();
    RECT ConsoleRect;
    GetWindowRect(console, &ConsoleRect);
    return { (int)(ConsoleRect.right - ConsoleRect.left), (int)(ConsoleRect.bottom - ConsoleRect.top) };
}
string Draw::Credits()
{
    
    
    SetConsoleFont(false, GetResolution());
    SetConsoleSize(GetResolution(), 24, 24);

    system("cls");
    
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

    string options[4][2];
    options[0][0] = "#play";
    options[0][1] = "Play";
    options[1][0] = "#edit";
    options[1][1] = "Edit";
    options[2][0] = "#res";
    options[2][1] = "Resolution";
    options[3][0] = "#quit";
    options[3][1] = "Quit";

    MainMenu choose;
    choose.Template(MenuTemplates::CHOOSE_EDIT_PLAY);
    choose.Create(options, 4);
    
    string selection = choose.Show();
    if (selection == "#res")
    {
        Resolution res = SetResolution();
        SetConsoleSize(res);
    }

    return selection;
}
Resolution Draw::SetResolution()
{
    string selection;
    
    system("cls");
    string resolution_options[6][2];
    resolution_options[0][0] = "#tiny";
    resolution_options[0][1] = "Tiny";
    resolution_options[1][0] = "#small";
    resolution_options[1][1] = "Small";
    resolution_options[2][0] = "#normal";
    resolution_options[2][1] = "Normal";
    resolution_options[3][0] = "#large";
    resolution_options[3][1] = "Large";
    resolution_options[4][0] = "#xl";
    resolution_options[4][1] = "Extra Large";
    resolution_options[5][0] = "#huge";
    resolution_options[5][1] = "Huge";

    MainMenu display_options;
    display_options.Template(MenuTemplates::DISPLAY_RESOLUTION);
    display_options.Create(resolution_options, 6);
    selection = display_options.Show();

    if (selection == "#tiny") {
        SetConsoleFont(false, Resolution::TINY);
        return Resolution::TINY;
    }
    
    if (selection == "#small") {
        SetConsoleFont(false, Resolution::SMALL);
        return Resolution::SMALL;
    }

    if (selection == "#normal") {
        SetConsoleFont(false, Resolution::NORMAL);
        return Resolution::NORMAL;
    }

    if (selection == "#large") {
        SetConsoleFont(false, Resolution::LARGE);
        return Resolution::LARGE;

    }
    
    if (selection == "#xl") {
        SetConsoleFont(false, Resolution::EXTRA_LARGE);
        return Resolution::EXTRA_LARGE;
    }

    if (selection == "#huge") {
        SetConsoleFont(false, Resolution::HUGE);
        return Resolution::HUGE;
    }

    SetConsoleFont(false, Resolution::NORMAL);
    return Resolution::NORMAL;

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
bool Draw::SetPacManFont(const bool pacman_font)
{
    Resolution resolution = GetResolution();
    SetConsoleFont(pacman_font, resolution);
    return true;
}
Resolution Draw::GetResolution(const int rows, const int cols)
{
    Coord console_size = GetConsoleSize();

    if (rows == 24)
    {
        switch (console_size.row)
        {
        case 305:
            return Resolution::TINY;
        case 401:
            return Resolution::SMALL;
        case 497:
            return Resolution::NORMAL;
        case 617:
            return Resolution::LARGE;
        case 689:
            return Resolution::EXTRA_LARGE;
        case 881:
            return Resolution::HUGE;
        default:
            return Resolution::NORMAL;
        }
    }

    if (rows == 29)
    {
        switch (console_size.row)
        {
        case 598:
            return Resolution::TINY;
        case 786:
            return Resolution::SMALL;
        case 974:
            return Resolution::NORMAL;
        case 1209:
            return Resolution::LARGE;
        case 1350:
            return Resolution::EXTRA_LARGE;
        case 1726:
            return Resolution::HUGE;
        default:
            return Resolution::NORMAL;
        }
    }
    
    return Resolution::NORMAL;
}