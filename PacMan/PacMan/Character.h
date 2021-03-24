#pragma once
#include "Coord.h"
#include "EnumsAndStatics.h"

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

	// methods
	bool IsReverseDirection(Direction new_direction);
	bool HasCollided(Character character);
	virtual void MoveTo(Coord coord, Direction direction);
	bool NotWall(const char map_char_content, const Direction direction);

	// getters
	int GetCurrentRow();
	int GetCurrentCol();
	int GetPreviousRow();
	int GetPreviousCol();
	Coord GetCurrentPosition();
	Coord GetPreviousPosition();
	Direction GetDirection();
	Direction GetPreviousDirection();

	// setters
	void SetDirection(Direction direction);
	void SetPreviousDirection(Direction direction);
	void SetSpawnPosition(Coord coord);
	void SetSpawnPosition(int row, int col);
	void SetCurrentPosition(Coord coord);
	void SetCurrentPosition(int row, int col);
	void SetPositions(int row, int col);
	void SetPreviousPosition(Coord coord);
	void SetPreviousPosition(int row, int col);
};
