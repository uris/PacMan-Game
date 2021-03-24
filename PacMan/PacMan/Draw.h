#pragma once

class Draw
{
public:
    //constructors
    Draw();

	//methods
    void SetColor(const int color);
    void ShowColors(const int colors);
    void CursorTopLeft(const int rows);
    void ShowConsoleCursor(const bool showFlag);
};
