#pragma once

class Draw
{
public:
    //constructors
    Draw();

    // destructors
    ~Draw();

	//methods
    void SetColor(int color);
    void ShowColors(int colors);
    void CursorTopLeft(int rows);
    void ShowConsoleCursor(bool showFlag);
};
