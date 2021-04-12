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
		selected_pen = '0';
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
	
	if (position.col < 0 || position.col > p_editor->p_scene->cols - 1)
		return false;
	
	if (position.row < 0 || position.row > p_editor->p_scene->rows - 1)
		return false;

	switch (p_editor->p_scene->p_map[position.row][position.col])
	{
	case char(Globals::red_ghost):
	case char(Globals::yellow_ghost):
	case char(Globals::blue_ghost):
	case char(Globals::pink_ghost):
	case char(Globals::one_way):
		return false;
	}

	return true;

}

void Cursor::SetEditor(Editor* p_editor)
{
	this->p_editor = p_editor;
}

char Cursor::Pen()
{
	return selected_pen;
}

void Cursor::SetPen(char pen)
{
	switch (pen)
	{
	case 's':
		selected_pen = 'S';
		pen_is_walls = false;
		pen_is_short_walls = false;
		break;
	case 't':
		selected_pen = 'T';
		pen_is_walls = false;
		pen_is_short_walls = false;
		break;
	case char(Globals::kHASH):
		pen_is_walls = true;
		pen_is_short_walls = false;
		index_wall_selected = -1;
		CycleLongWalls();
		break;
	case char(Globals::kAT) :
		pen_is_walls = false;
		pen_is_short_walls = true;
		index_short_wall_selected = -1;
		CycleShortWalls();
		break;
	case '.':
		selected_pen = char(Globals::pellet);
		pen_is_walls = false;
		pen_is_short_walls = false;
		break;
	default:
		selected_pen = pen;
		pen_is_walls = false;
		pen_is_short_walls = false;
		break;
	}
}

void Cursor::CycleLongWalls(bool increment)
{

	if (increment)
	{
		if (index_wall_selected < 20)
			index_wall_selected++;

		if (index_wall_selected == 20)
			index_wall_selected = 0;
	}
	else
	{
		if (index_wall_selected > 0)
			index_wall_selected--;

		if (index_wall_selected == 0)
			index_wall_selected = 20;
	}

	char walls[20] = {
		char(Globals::lwall_187),
		char(Globals::lwall_188),
		char(Globals::lwall_200),
		char(Globals::lwall_203),
		char(Globals::lwall_184),
		char(Globals::lwall_213),
		char(Globals::lwall_212),
		char(Globals::lwall_209),
		char(Globals::lwall_155),
		char(Globals::lwall_210),
		char(Globals::lwall_183),
		char(Globals::lwall_214),
		char(Globals::lwall_220),
		char(Globals::lwall_221),
		char(Globals::lwall_222),
		char(Globals::lwall_223),
		char(Globals::lwall_201),
		char(Globals::lwall_205),
		char(Globals::lwall_215),
		char(Globals::lwall_182),
	};

	for (int i = 0; i < 20; i++)
	{
		if (index_wall_selected == i)
		{
			selected_pen = walls[i];
			break;
		}
	}

}

void Cursor::CycleShortWalls(bool increment)
{
	if (increment)
	{
		if (index_short_wall_selected < 20)
			index_short_wall_selected++;

		if (index_short_wall_selected == 20)
			index_short_wall_selected = 0;
	}
	else
	{
		if (index_short_wall_selected > 0)
			index_short_wall_selected--;

		if (index_short_wall_selected == 0)
			index_short_wall_selected = 20;
	}

	char walls[20] = {
		char(Globals::lwall_180), // Top Cap
		char(Globals::lwall_192), // Right cap
		char(Globals::lwall_197), // Bottom Cap
		char(Globals::lwall_217), // Left Cap
		char(Globals::lwall_193), // Horizontal Straight
		char(Globals::lwall_195), // Vertical Straight
		char(Globals::lwall_191), // bottom T
		char(Globals::lwall_194), // Top T
		char(Globals::lwall_196), // Left T
		char(Globals::lwall_218), // Right T
		char(Globals::lwall_179), // Intersection
		char(Globals::lwall_190), // Island
		char(Globals::lwall_181), // Bottom Right Corner
		char(Globals::lwall_198), // Top Left Corner
		char(Globals::lwall_207), // Top Right Corner
		char(Globals::lwall_216), // Bottom Left Corner
		char(Globals::lwall_201), // Inner Bottom Right Corner
		char(Globals::lwall_205), // Inner Top Left Corner
		char(Globals::lwall_215), // Inner Bottom Left Corner
		char(Globals::lwall_182), // Inner Top Right  Corner
	};

	for (int i = 0; i < 20; i++)
	{
		if (index_short_wall_selected == i)
		{
			selected_pen = walls[i];
			break;
		}
	}

}