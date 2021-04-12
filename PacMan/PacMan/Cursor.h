#pragma once
#include "Character.h"
#include "EnumsAndStatics.h"

class Cursor : public Character
{
	class Editor* p_editor = nullptr; // fwd declare class

private:
	EditMode mode = EditMode::HOVER;
	bool blink_on = false;
	char selected_pen = '0';
	
	
	

public:
	char content_now =' ';
	// long walls
	bool pen_is_walls = false;
	int index_wall_selected = -1;
	// short walls
	bool pen_is_short_walls = false;
	int index_short_wall_selected = -1;
	
	// constructor
	Cursor();

	// destructor
	~Cursor();

	// methods
	void SpawnCursor(int map_width, int map_height);
	bool IsEditable(Direction direction);
	bool IsInBounds(Direction direction);
	void CycleLongWalls(bool increment = true);
	void CycleShortWalls(bool increment = true);

	// Getters
	bool IsEditing();
	char Pen();
	bool Blink();

	// Setters
	void SetBlink();
	void MoveCursor();
	void SetEditor(Editor* p_editor);
	void ChangeMode(bool reset_pen = true);
	void SetPen(char pen);

};