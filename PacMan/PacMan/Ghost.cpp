#include "Ghost.h"
#include "Coord.h"
#include "Enums.h"

using namespace std;

// constructors
Ghost::Ghost()
{
    name = Ghosts::RED;
    ghost = 'R';
    roam_target = { 24, 45 };
    chase_modifier = { 0, 0 };
    name = Ghosts::RED;
    color = 71;
    wait = 15;
};

Ghost::Ghost(Ghosts ghost)
{
    // todo: adjust roam target dynamically to size of level map
    switch (ghost)
    {
    case Ghosts::RED:
        name = Ghosts::RED;
        roam_target = { 24, 45 };
        chase_modifier = { 0, 0 };
        this->ghost = 'R';
        color = 71;
        wait = 15;
        break;
    case Ghosts::YELLOW:
        name = Ghosts::YELLOW;
        roam_target = { 24, 2 };
        chase_modifier = { 0, 3 };
        this->ghost = 'Y';
        color = 367;
        wait = 30;
        break;
    case Ghosts::BLUE:
        name = Ghosts::BLUE;
        roam_target = { -3, 2 };
        chase_modifier = { 0, -3 };
        this->ghost = 'B';
        color = 435;
        wait = 45;
        break;
    case Ghosts::PINK:
        name = Ghosts::PINK;
        roam_target = { -3, 45 };
        chase_modifier = { -3, 0 };
        this->ghost = 'P';
        color = 479;
        wait = 60;
        break;
    default:
        name = Ghosts::RED;
        roam_target = { 24, 45 };
        chase_modifier = { 0, 0 };
        name = Ghosts::RED;
        this->ghost = 'R';
        color = 71;
        wait = 15;
        break;
    }
}

// destructors
Ghost::~Ghost() {};

// encapsulation

char Ghost::GetPreviousSqaureContent()
{
    return square_content_prior;
}

void Ghost::SetPreviousSqaureContent(char content)
{
    square_content_prior = content;
}

char Ghost::GetContentCurrent()
{
    return square_content_now;
}

void Ghost::SetContentCurrent(char content)
{
    square_content_now = content;
}

Mode Ghost::GetMode()
{
    return mode;
}

void Ghost::SetMode(Mode mode)
{
    this->mode = mode;
}

bool Ghost::IsEdible()
{
    return is_edible;
}

void Ghost::SetEdible(bool edible)
{
    is_edible = edible;
}

bool Ghost::FlashBlue()
{
    return color_on;
}

void Ghost::SetFlashBlue(bool flash)
{
    color_on = flash;
}

int Ghost::GetColor()
{
    return color;
}

void Ghost::SetColor(int color)
{
    this->color = color;
}

int Ghost::GetWait()
{
    return wait;
}

void Ghost::SetWait(int wait)
{
    this->wait = wait;
}

void Ghost::DecreaseWait()
{
    wait--;
}

void Ghost::SetSkipTurn(bool skip_turn)
{
    this->skip_turn = skip_turn;
}

bool Ghost::SkipTurn()
{
    return skip_turn;
}

void Ghost::SetLookAhead(int moves)
{
    look_ahead = moves;
}

int Ghost::GetLookAhead()
{
    return look_ahead;
}

void Ghost::SpawnGhost(bool player_died)
{
    // if player died nned to stagger exits from spawn area
    switch (name)
    {
    case Ghosts::RED:
        wait = 15;
        break;
    case Ghosts::YELLOW:
        wait = player_died ? 30 : 15;
        break;
    case Ghosts::BLUE:
        wait = player_died ? 45 : 15;
        break;
    case Ghosts::PINK:
        wait = player_died ? 60 : 15;
        break;
    default:
        wait = 15;
        break;
    }
    
    previous_position = current_position;
    current_position = spawn_position;
    square_content_now = ' ';
    square_content_prior = ' ';
    current_direction = Direction::UP;
    previous_direction = Direction::UP;
    mode = Mode::SPAWN;
    run_first_move = true;
    is_edible = false;
    color_on = false;
    wait = 15;
    skip_turn = false;
}

void Ghost::SetSpawnTarget(Coord spawn_target)
{
    this->spawn_target = spawn_target;
}

void Ghost::SetSpawnTarget(int row, int col)
{
    spawn_target.row = row;
    spawn_target.col = col;
}

Coord Ghost::GetSpawnTarget()
{
    return spawn_target;
}

char Ghost::GhostChar()
{
    return ghost;
}

void Ghost::SetReverseMove(bool reverse)
{
    run_first_move = reverse;
}

bool Ghost::ReverseMove()
{
    return run_first_move;
}

Coord Ghost::GetChaseModifier()
{
    return chase_modifier;
}

Coord Ghost::GetRoamTarget()
{
    return roam_target;
}

Ghosts Ghost::Name()
{
    return name;
}
