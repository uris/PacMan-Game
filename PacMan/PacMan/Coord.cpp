#include "Coord.h"
#include <algorithm> // string transforms

// constructors
Coord::Coord()
{
	row = 0;
	col = 0;
}

Coord::Coord(int row, int col)
{
	this->row = row;
	this->col = col;
}

Coord::Coord(Coord coord, Direction direction)
{
	row = coord.row;
	col = coord.col;
	switch (direction)
	{
	case Direction::UP:
		row--;
		break;
	case Direction::RIGHT:
		col++;
		break;
	case Direction::DOWN:
		row++;
		break;
	case Direction::LEFT:
		col--;
		break;
	case Direction::NONE:
		break;
	}
}

// destructors
Coord::~Coord()
{

}

// methods
bool Coord::IsSame(Coord coord)
{
	return(coord.row == row && coord.col == col ? true : false);
}

void Coord::SetTo(Coord coord)
{
	row = coord.row;
	col = coord.col;
}

void Coord::SetTo(Coord coord, Direction direction)
{
	row = coord.row;
	col = coord.col;
	switch (direction)
	{
	case Direction::UP:
		row--;
		break;
	case Direction::RIGHT:
		col++;
		break;
	case Direction::DOWN:
		row++;
		break;
	case Direction::LEFT:
		col--;
		break;
	case Direction::NONE:
		break;
	}
}

int Coord::DistanceTo(Coord coord)
{
	// return distance from one coord to another
	return (abs(col - coord.col) + abs(row - coord.row));
}

int Coord::DistanceTo(Coord coord, Coord modifier)
{
	// overload to return the distance to a coord with a specified modifier
	return (abs(col - (coord.col + modifier.col)) + abs(row - (coord.row + modifier.row)));
}