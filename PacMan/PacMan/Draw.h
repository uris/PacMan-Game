#pragma once
#include <string>
#include "EnumsAndStatics.h"
#include "Coord.h"

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
    static string WriteEmptyLine(int height, int length);
    static bool SetConsoleFont(const bool pacman_font, const Resolution resolution);
    static bool SetPacManFont(const bool pacman_font);
    static void SetConsoleSize(const Resolution resolution, const int rows = 29, const int cols = 47);
    static Coord GetConsoleSize();
    static Resolution SetResolution();
    static Resolution GetResolution();
};
