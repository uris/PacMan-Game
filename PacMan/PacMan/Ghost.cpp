#include "Ghost.h"

using namespace std;

// constructors
Ghost::Ghost()
{
    ghost = 'R';
    square_content_now = ' ';
    square_content_prior = ' ';
    square_content_now = ' ';
};

Ghost::Ghost(Ghosts ghost)
{
    switch (ghost)
    {
    case Ghosts::RED:
        this->ghost = 'R';
        break;
    case Ghosts::YELLOW:
        this->ghost = 'Y';
        break;
    case Ghosts::BLUE:
        this->ghost = 'B';
        break;
    case Ghosts::PINK:
        this->ghost = 'P';
        break;
    default:
        this->ghost = 'R';
        break;
    }
    square_content_now = ' ';
    square_content_prior = ' ';
    square_content_now = ' ';
}

// destructors
Ghost::~Ghost() {};

//methods
char Ghost::GetContentBeforeEntering()
{
    return square_content_now;
}

void Ghost::SetContentBeforeEntering(char content)
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
    this->wait = wait
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
