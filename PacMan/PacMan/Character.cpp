#include "Character.h"
#include "Coord.h"

// constructors
Character::Character() {};

// destructors
Character::~Character() {};

// methods

bool IsReverse(const Direction& curr_direction, const Direction& new_direction)
{
	switch (new_direction)
	{
	case Direction::UP:
		return (curr_direction == Direction::DOWN ? true : false);
		break;
	case Direction::RIGHT:
		return (curr_direction == Direction::LEFT ? true : false);
		break;
	case Direction::DOWN:
		return (curr_direction == Direction::UP ? true : false);
		break;
	case Direction::LEFT:
		return (curr_direction == Direction::RIGHT ? true : false);
		break;
	}
	return false;
}

// encapsulate
bool Character::IsReverseDirection(Direction new_direction)
{
    switch (new_direction)
    {
    case Direction::UP:
        return (current_direction == Direction::DOWN ? true : false);
        break;
    case Direction::RIGHT:
        return (current_direction == Direction::LEFT ? true : false);
        break;
    case Direction::DOWN:
        return (current_direction == Direction::UP ? true : false);
        break;
    case Direction::LEFT:
        return (current_direction == Direction::RIGHT ? true : false);
        break;
    }
    return false;
}

bool  Character::HasCollided(Character character)
{
    return (current_position.IsSame(character.current_position));
}

void Character::SetDirection(Direction direction)
{
	current_direction = direction;
}

void Character::SetPreviousDirection(Direction direction)
{
	previous_direction = direction;
}

Direction Character::GetDirection()
{
	return(current_direction);
}

Direction Character::GetPreviousDirection()
{
	return(previous_direction);
}

void Character::SetPositions(Coord coord)
{
	spawn_position = current_position = previous_position = coord;
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

Coord Character::GetCurrentPosition()
{
	return(current_position);
}

int Character::GetCurrentRow()
{
	return(current_position.row);
}

int Character::GetCurrentCol()
{
	return(current_position.col);
}

Coord Character::GetPreviousPosition()
{
	return(previous_position);
}

int Character::GetPreviousRow()
{
	return(previous_position.row);
}

int Character::GetPreviousCol()
{
	return(previous_position.col);
}

void Character::MoveTo(Coord coord)
{
	current_position = coord;
}

void Character::MoveTo(int row, int col)
{
	current_position.row = row;
	current_position.col = col;
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

void Character::MoveTo(int row, int col, Direction direction)
{
	current_position.row = row;
	current_position.col = col;
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