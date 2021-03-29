#include "Character.h"

// constructors
Character::Character() {};


// methods
bool Character::IsReverseDirection(Direction new_direction)
{
	switch (new_direction)
	{
	case Direction::UP:
		return (current_direction == Direction::DOWN ? true : false);
	case Direction::RIGHT:
		return (current_direction == Direction::LEFT ? true : false);
	case Direction::DOWN:
		return (current_direction == Direction::UP ? true : false);
	case Direction::LEFT:
		return (current_direction == Direction::RIGHT ? true : false);
	}
	return false;
}

void Character::MoveTo(Coord coord, Direction direction)
{
	current_position = coord;
	switch (direction)
	{
	case Direction::UP:
		current_position.row--;
		break;
	case Direction::RIGHT:
		current_position.col++;
		break;
	case Direction::DOWN:
		current_position.row++;
		break;
	case Direction::LEFT:
		current_position.col--;
		break;
	case Direction::NONE:
		break;
	}
}


// getters
Direction Character::GetDirection()
{
	return(current_direction);
}

Direction Character::GetPreviousDirection()
{
	return(previous_direction);
}

Coord Character::GetCurrentPosition()
{
	return(current_position);
}

Coord Character::GetPreviousPosition()
{
	return(previous_position);
}

int Character::GetCurrentRow()
{
	return(current_position.row);
}

int Character::GetCurrentCol()
{
	return(current_position.col);
}

int Character::GetPreviousRow()
{
	return(previous_position.row);
}

int Character::GetPreviousCol()
{
	return(previous_position.col);
}


// Setters
void Character::SetDirection(Direction direction)
{
	current_direction = direction;
}

void Character::SetPreviousDirection(Direction direction)
{
	previous_direction = direction;
}

void Character::SetPositions(int row, int col)
{
	spawn_position.row = current_position.row = previous_position.row = row;
	spawn_position.col = current_position.col = previous_position.col = col;
}

void Character::SetSpawnPosition(Coord coord)
{
	spawn_position = coord;
}

void Character::SetSpawnPosition(int row, int col)
{
	spawn_position.row = row;
	spawn_position.col = col;
}

void Character::SetCurrentPosition(Coord coord)
{
	current_position = coord;
}

void Character::SetCurrentPosition(int row, int col)
{
	current_position.row = row;
	current_position.col = col;
}

void Character::SetPreviousPosition(Coord coord)
{
	previous_position = coord;
}

void Character::SetPreviousPosition(int row, int col)
{
	previous_position.row = row;
	previous_position.col = col;
}