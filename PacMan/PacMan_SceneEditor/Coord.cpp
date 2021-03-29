#include "Coord.h"

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

// methods
bool Coord::IsSame(const Coord& coord)
{
	return(coord.row == row && coord.col == col ? true : false);
}

void Coord::SetTo(const Coord& coord)
{
	row = coord.row;
	col = coord.col;
}

void Coord::SetTo(const Coord& coord, const Direction direction)
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