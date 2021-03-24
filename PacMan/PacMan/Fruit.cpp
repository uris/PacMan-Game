#include "Fruit.h"
#include "Coord.h"
#include "EnumsAndStatics.h"
#include <iomanip> // abs function

using namespace std;

// constructors
Fruit::Fruit(char fruit)
{
    this->fruit = fruit;
};

Fruit::Fruit(char fruit, Coord roam_points[], int roam_points_size)
{
    this->fruit = fruit;
};


// methods
int Fruit::DistanceToRoamTarget()
{
    // overload to return the distance to a coord with a specified modifier
    return (abs(current_position.col - roam_target.col) + abs(current_position.row - roam_target.row));
}

bool Fruit::PlayerCollision(Coord player_coord)
{
    return (current_position.row == player_coord.row && current_position.col == player_coord.col) ? true : false;

}

void Fruit::MoveFruit(const Coord player_coord, const Direction direction, const char map_content)
{
    previous_position = current_position;
    square_content_prior = square_content_now;
    square_content_now = map_content;

    switch (direction)
    {
    case Direction::UP: //up
        current_position.row--;
        current_direction = Direction::UP;
        break;
    case Direction::RIGHT: // right
        current_position.col++;
        current_direction = Direction::RIGHT;
        break;
    case Direction::DOWN: // down
        current_position.row++;
        current_direction = Direction::DOWN;
        break;
    case Direction::LEFT: // left
        current_position.col--;
        current_direction = Direction::LEFT;
        break;
    default:
        break;
    }

    // if the sqaure the ghost moved into is the player
    square_content_prior == Globals::player ? square_content_prior = ' ' : square_content_prior;

    // if the mosnter is over the player save a blank space to buffer
    PlayerCollision(player_coord) ? square_content_now = ' ' : square_content_now;

}

// encapsulation
char Fruit::GetPreviousSqaureContent()
{
    return square_content_prior;
}

void Fruit::SetPreviousSqaureContent(char content)
{
    square_content_prior = content;
}

char Fruit::GetContentCurrent()
{
    return square_content_now;
}

void Fruit::SetContentCurrent(char content)
{
    square_content_now = content;
}

void Fruit::SetContentCurrent(bool same)
{
    square_content_now = square_content_now;
}

int Fruit::GetColor()
{
    return color;
}

int Fruit::GetWait()
{
    return wait;
}

void Fruit::SetWait(int wait)
{
    this->wait = wait;
}

void Fruit::DecreaseWait()
{
    wait--;
}

void Fruit::SpawnFruit()
{
    previous_position = current_position;
    current_position = spawn_position;
    square_content_now = ' ';
    square_content_prior = ' ';
    current_direction = Direction::LEFT;
    previous_direction = Direction::LEFT;
    mode = Mode::ROAM;
}

char Fruit::FruitChar()
{
    return fruit;
}

Coord Fruit::GetRoamTarget()
{
    return roam_target;
}
