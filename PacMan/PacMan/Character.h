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
	bool run_first_move = true;

public:
	// forward class declared as pointer to game object
	class Game* p_game = nullptr;

	// constructors
	Character();

	// methods
	bool IsReverseDirection(Direction new_direction);

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
	void Teleport();
	void Teleport(Coord& position);
	Direction RandomMove(const bool is_ghost);
};
