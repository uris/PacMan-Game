#include "Game.h"
#include <iomanip> // abs function
#include <iostream>

using namespace std;
using namespace std::chrono;


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

void Character::Teleport()
{
	
	if (current_position == p_game->p_level->tp_1)
	{
		current_position = p_game->p_level->tp_2;
	}
	else if (current_position == p_game->p_level->tp_2)
	{
		current_position = p_game->p_level->tp_1;
	}
}

// use this for AI teleport since we're not using simulated current position of the character
void Character::Teleport(Coord& new_position)
{

	if (new_position == p_game->p_level->tp_1)
	{
		new_position = p_game->p_level->tp_2;
	}
	else if (new_position == p_game->p_level->tp_2)
	{
		new_position = p_game->p_level->tp_1;
	}
}

Direction Character::RandomMove(const bool is_ghost)
{
	// Do teleport if on teleport position
	Teleport();

	Direction new_direction = Direction::NONE;
	Coord next_move;

	// first move when on the run is ALWAYS to reverse direction which is ALWAYS a valid move
	if (is_ghost)
	{
		if (run_first_move) {
			switch (current_direction)
			{
			case Direction::UP:
				new_direction = Direction::DOWN;
				break;
			case Direction::RIGHT:
				new_direction = Direction::LEFT;
				break;
			case Direction::DOWN:
				new_direction = Direction::UP;
				break;
			case Direction::LEFT:
				new_direction = Direction::RIGHT;
				break;
			}
			run_first_move = false;
			return new_direction;
		}
	}
		

	int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
	srand(seed);
	do
	{
		int random_number = rand() % 4; //generate random number to select from the 4 possible options
		new_direction = static_cast<Direction>(random_number);
		next_move = current_position + new_direction;
	} while (!p_game->p_level->NotWall(next_move, new_direction) || IsReverseDirection(new_direction));

	return new_direction;
}