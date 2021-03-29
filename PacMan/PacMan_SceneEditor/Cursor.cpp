# include "Cursor.h"
# include "Editor.h"

// constructor
Cursor::Cursor()
{
	current_position.row = 1;
	current_position.col = 1;
	previous_position.row = 1;
	previous_position.col = 1;
	current_direction = Direction::NONE;
}

// destructor
Cursor::~Cursor()
{
	if (p_editor)
	{
		p_editor = nullptr;
	}
}

// methods
void Cursor::SpawnCursor(int map_width, int map_height)
{
	current_position.row = current_position.col = 1;
}

void Cursor::ChangeMode(bool reset_pen)
{
	if (reset_pen)
	{
		seleced_pen = '0';
	}
	mode == EditMode::HOVER ? mode = EditMode::EDIT : mode = EditMode::HOVER;
	
	//mode == EditMode::HOVER ? reset_pen ? seleced_pen = '0' : seleced_pen;
}

bool Cursor::IsEditing()
{
	return (mode == EditMode::EDIT ? true : false);
}

bool Cursor::Blink()
{
	return blink_on;
}

void Cursor::SetBlink()
{
	blink_on = !blink_on;
}

void Cursor::MoveCursor()
{
	
	
	
	if (current_direction != Direction::NONE)
	{
		Coord next_position;
		next_position.SetTo(current_position, current_direction);
		previous_position = current_position;
		
		switch (current_direction)
		{
		case Direction::UP: //up
			if (!IsInBounds(Direction::UP)) {
				next_position = current_position;
			}
			break;
		case Direction::RIGHT: // right
			if (!IsInBounds(Direction::RIGHT)) {
				next_position = current_position;
			}
			break;
		case Direction::DOWN: // down
			if (!IsInBounds(Direction::DOWN)) {
				next_position = current_position;
			}
			break;
		case Direction::LEFT: // left
			if (!IsInBounds(Direction::LEFT)) {
				next_position = current_position;
			}
			break;
		}

		current_direction = Direction::NONE;
		current_position = next_position;
	}
	
	
}

bool Cursor::IsEditable(Direction direction)
{
	return true;
}

bool Cursor::IsInBounds(Direction direction)
{
	Coord position(current_position, direction);
	if (position.col < 1 || position.col > p_editor->p_scene->cols - 2)
		return false;
	if (position.row < 1 || position.row > p_editor->p_scene->rows - 2)
		return false;
	return true;

}

void Cursor::SetEditor(Editor* p_editor)
{
	this->p_editor = p_editor;
}

char Cursor::Pen()
{
	return seleced_pen;
}

void Cursor::SetPen(char pen)
{
	switch (pen)
	{
	case 's':
		seleced_pen = 'S';
		break;
	case 't':
		seleced_pen = 'T';
		break;
	case '.':
		seleced_pen = Globals::pellet;
		break;
	default:
		seleced_pen = pen;
		break;
	}
}