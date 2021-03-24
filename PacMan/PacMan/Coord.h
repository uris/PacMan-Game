#pragma once
#include "EnumsAndStatics.h"

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
	bool IsSame(const Coord& coord);
	void SetTo(const Coord& coord);
	void SetTo(const Coord& coord, const Direction direction);
};
