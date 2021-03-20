#pragma once
#include "Coord.h"

class Character
{

protected:
	
	// variables
	Coord current_position;
	Coord spawn_position;
	Coord previous_position;
	Direction current_direction = Direction::LEFT;
	Direction previous_direction = Direction::LEFT;

public:
	// constructors
	Character();

	// destructors
	~Character();

	// methods
	bool IsReverseDirection(Direction new_direction);
	bool HasCollided(Character character);
	void MoveTo(Coord coord);
	void MoveTo(int row, int col);
	void MoveTo(Coord coord, Direction direction);
	void MoveTo(int row, int col, Direction direction);
	void SetDirection(Direction direction);
	Direction GetDirection();
	Direction GetPreviousDirection();
	void SetPositions(Coord coord);
	void SetPositions(int row, int col);
	void SetPreviousDirection(Direction direction);
	void SetSpawnPosition(Coord coord);
	void SetSpawnPosition(int row, int col);
	void SetCurrentPosition(Coord coord);
	void SetCurrentPosition(int row, int col);
	Coord GetCurrentPosition();
	Coord GetPreviousPosition();
	int GetCurrentRow();
	int GetCurrentCol();
	int GetPreviousRow();
	int GetPreviousCol();
	void SetPreviousPosition(Coord coord);
	void SetPreviousPosition(int row, int col);
};
