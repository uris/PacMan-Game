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

	// destructors
	~Coord();

	// methods
	bool IsSame(Coord coord);
	void SetTo(Coord coord);
	void SetTo(Coord coord, Direction direction);
	int DistanceTo(Coord coord);
	int DistanceTo(Coord coord, Coord modifier);
};
