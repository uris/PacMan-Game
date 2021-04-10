#include "Coord.h"
#include <algorithm>

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

bool Coord::operator==(const Coord& coord) const
{
	return(coord.row == row && coord.col == col ? true : false);
}

bool Coord::operator!=(const Coord& coord) const
{
	return(coord.row == row && coord.col == col ? false : true);
}

Coord Coord::operator+(const Coord& coord) const
{
	return Coord(row + coord.row, col + coord.col);
}

Coord Coord::operator-(const Coord& coord) const
{
	return Coord(row - coord.row, col - coord.col);
}

int Coord::operator%(const Coord& coord) const
{
	return (abs(row - coord.row) + abs(col - coord.col));
}

Coord Coord::operator+(const Direction& direction) const
{
	switch (direction)
	{
	case Direction::UP:
		return Coord(row-1, col);
		break;
	case Direction::RIGHT:
		return Coord(row, col + 1);
		break;
	case Direction::DOWN:
		return Coord(row + 1, col);
		break;
	case Direction::LEFT:
		return Coord(row, col - 1);
	default:
		return Coord(row, col);
	}
}

// methods

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