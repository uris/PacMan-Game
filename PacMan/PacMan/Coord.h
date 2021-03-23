#pragma once
#include "Enums.h"

class Coord
{
public:
	int row;
	int col;

	// constructors
	Coord();
	Coord(int row, int col);
	Coord(Coord coord, Direction direction);

	// methods
	bool IsSame(Coord coord);
	void SetTo(Coord coord);
	void SetTo(Coord coord, Direction direction);
};
