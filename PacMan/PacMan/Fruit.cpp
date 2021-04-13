#include "Game.h"
#include <iomanip> // abs function

using namespace std;

// constructors
Fruit::Fruit()
{
  // nothing for now  
};

Fruit::Fruit(Fruits fruit_type)
{
    this->fruit_type = fruit_type;
};

Fruit::~Fruit()
{
    p_game = nullptr;
}

// methods
void Fruit::SpawnFruit()
{
    // set a random exit at a teleport point
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);
    int random_number = rand() % 2;
    if (random_number == 0) {
        exit_target = p_game->p_level->tp_1;
    }
    else {
        exit_target = p_game->p_level->tp_2;
    }
    
    spawn_count++;
    previous_position = current_position;
    current_position = spawn_position;
    square_content_now = ' ';
    square_content_prior = ' ';
    current_direction = Direction::LEFT;
    previous_direction = Direction::LEFT;
    spawn_position = spawn_position;
    is_in_level = true;
    skip_turn = false;
    wait = 0;
    p_game->p_level->p_map[current_position.row][current_position.col] = Globals::fruit;
}

void Fruit::KillFruit()
{
    // remove the fruit from the map replacing with previous content
    p_game->p_level->p_map[current_position.row][current_position.col] = square_content_prior;
    is_in_level = false;
    ticks = 0; // restart counter
}

void Fruit::ResetFruit()
{
    is_in_level = false;
    skip_turn = false;
    wait = Globals::fDelay;
    ticks = 0;
    not_eaten = true;
    spawn_count = 0;
    move_count = 0;

}

void Fruit::MoveFruit(const Direction direction, const char map_content)
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

    // if the sqaure the fruit moved into is the player
    square_content_prior == Globals::player ? square_content_prior = ' ' : square_content_prior;

    // if the fruit is over the player save a blank space to buffer
    current_position == p_game->p_player->GetCurrentPosition() ? square_content_now = ' ' : square_content_now;

}

Direction Fruit::RandomFruitMove()
{
    Direction new_direction = Direction::NONE;
    Coord next_move;

    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (p_game->p_level->IsTeleport(current_position)) // if on teleportkeep the same direction
        current_direction == Direction::LEFT ? new_direction = Direction::LEFT : new_direction = Direction::RIGHT;
    else // else choose a valid random direction
    {
        do
        {
            int random_number = rand() % 4; //generate random number to select from the 4 possible options
            new_direction = static_cast<Direction>(random_number);
            next_move.SetTo(current_position, new_direction);
        } while (!p_game->p_level->NotWall(next_move, new_direction) || IsReverseDirection(new_direction));
    }
    return new_direction;
}

int Fruit::MakeFruitMove()
{
    Direction best_move = Direction::NONE; // will store the next move

    if (!is_in_level && not_eaten)
    {
        ticks++;
        if (ticks % Globals::fSpawnRate == 0 && spawn_count < max_spawns)
        {
            SpawnFruit();
        }
        return 0;
    }

    if (p_game->IsGameOver() || current_position == p_game->p_player->GetCurrentPosition())
    {
        return 0; //  (no move)
    }

    if (wait > 0)
    {
        wait--; //
        return 0; // delay the fruit move (will appear slower than every other character)
    }

    if (move_count < max_moves) // make random moves for max moves number of times
    {
        best_move = RandomFruitMove(); // get the random move
        char map_content = FruitContentNow(best_move);
        MoveFruit(best_move, map_content); // make the move
        move_count++;
        wait = Globals::fDelay; // reset the delay to slow down the fruit by three ticks
        return 0;
    }

    if (move_count >= max_moves && current_position == exit_target)
    {
        KillFruit();
        return 0;
    }

    if (!skip_turn)  // run recursive AI for getting to target exit area
    {
        int score = 0, best_score = 1000;
        Coord next_move, prior_position;
        Direction new_direction = Direction::NONE;

        // Do teleport if on teleport
        Teleport(current_position, current_direction);

        for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
        {

            new_direction = static_cast<Direction>(i); // set the direction we will get best move for
            next_move.SetTo(current_position, new_direction);

            if (p_game->p_level->NotWall(next_move, new_direction) && !IsReverseDirection(new_direction)) // check for next available square
            {
                // it's a viable move to position so set new coords of fruit
                prior_position = current_position;
                current_position = next_move;

                // calculate distance score based on new coords
                int score = current_position % exit_target; // get the score for the move

                // revert fruit coords back
                current_position = prior_position;

                // if the new move score gets the fruit closer to the target coord
                if (score < best_score)
                {
                    best_score = score; // set new best score
                    best_move = new_direction; // set new best move direction
                }
            }
        }

        char map_content = FruitContentNow(best_move); // if the content is a ghosts set the contnet value of the gohst to the other ghosts content
        MoveFruit(best_move, map_content); // do the move
        wait = Globals::fDelay; // reset the delay to slow down the fruit by three ticks
        return 0;
    }
    return 0; // clean exit
}

char Fruit::FruitContentNow(Direction best_move)
{
    Coord move_into(current_position, best_move);
    char map_content = p_game->p_level->p_map[move_into.row][move_into.col];
    switch (map_content)
    {
    case Globals::red_ghost:
        return p_game->p_ghosts[0]->GetContentCurrent();
        break;
    case Globals::yellow_ghost:
        return p_game->p_ghosts[1]->GetContentCurrent();
        break;
    case Globals::blue_ghost:
        return p_game->p_ghosts[2]->GetContentCurrent();
        break;
    case Globals::pink_ghost:
        return p_game->p_ghosts[3]->GetContentCurrent();
        break;
    case Globals::player:
        return p_game->p_player->GetMovedIntoSquareContents();
        break;
    default:
        return map_content;
        break;
    }
}

void Fruit::Teleport(Coord& fruit_position, Direction& fruit_direction)
{
    if (p_game->p_level->IsTeleport(fruit_position))
    {
        if (fruit_position.col == 0 && fruit_direction == Direction::LEFT)
        {
            fruit_position.col = p_game->p_level->cols - 1;
        }
        if (fruit_position.col == p_game->p_level->cols - 1 && fruit_direction == Direction::RIGHT)
        {
            fruit_position.col = 0;
        }
    }
}

void Fruit::CoutFruit()
{
    Draw::SetColor(Globals::cRED);
    switch (fruit_type)
    {
    case Fruits::CHERRY:
        cout << char(Globals::fCherry);
        break;
    case Fruits::STRAWBERRY:
        cout << char(Globals::fStrawberry);
        break;
    case Fruits::APPLE:
        cout << char(Globals::fApple);
        break;
    case Fruits::PEAR:
        cout << char(Globals::fPear);
        break;
    default:
        cout << char(Globals::fCherry);
    }
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

Fruits Fruit::FruitType()
{
    return fruit_type;
}

Coord Fruit::GetExitTarget()
{
    return exit_target;
}

bool Fruit::GameRefIsSet()
{
    return (p_game ? true : false);
}

void Fruit::SetGameRef(Game* p_game)
{
    this->p_game = p_game;
}

bool Fruit::FruitActive()
{
    return is_in_level;
}

void Fruit::SetFruitType(Fruits fruit_type)
{
    this->fruit_type = fruit_type;
}
