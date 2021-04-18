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
    current_direction = Direction::LEFT;
    previous_direction = Direction::LEFT;
    spawn_position = spawn_position;
    move_count = 0;
    is_in_level = true;
    skip_turn = false;
    wait = 0;
    //p_game->p_level->p_map[current_position.row][current_position.col] = Globals::fruit;
}

void Fruit::KillFruit(bool eaten)
{
    if (is_in_level)
    {
        is_in_level = false;
        move_count = 0;
        ticks = 0; // restart counter
    }
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

void Fruit::MoveFruit(const Direction direction)
{
    previous_position = current_position;

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

}

int Fruit::MakeFruitMove()
{
    Direction best_move = Direction::NONE; // will store the next move

    // if eaten already no more activity
    if (!not_eaten)
    {
        return 0;
    }

    // if it is not in the level currently and has not been eaten, add ticks and spawn
    if (!is_in_level && not_eaten)
    {
        if (spawn_count < max_spawns)
        {
            ticks++;
            if (ticks % Globals::fSpawnRate == 0)
            {
                SpawnFruit();
            }
        }
        return 0;
    }

    // game over or is eaten means it does not move
    if (p_game->IsGameOver() || current_position == p_game->p_player->GetCurrentPosition())
    {
        return 0; //  (no move)
    }

    // if it's in a wait state, decrease wait and exit
    if (wait > 0)
    {
        wait--; //
        return 0; // delay the fruit move (will appear slower than every other character)
    }

    // if it's in exit mode and has reached it's exit target
    if (move_count >= max_moves && current_position == exit_target)
    {
        KillFruit();
        return 0;
    }

    // if it's roaming randomly ie has not hit the max random moves
    if (move_count < max_moves) // make random moves for max moves number of times
    {
        // Do teleport if on teleport
        Teleport();
        
        best_move = RandomMove(false); // get the random move
        MoveFruit(best_move); // make the move
        move_count++;
        wait = Globals::fDelay; // reset the delay to slow down the fruit by three ticks
        return 0;
    }

    if (!skip_turn)  // run recursive AI for getting to target exit area
    {
        int score = 0, best_score = 1000;
        Coord next_move, prior_position;
        Direction new_direction = Direction::NONE;

        // Do teleport if on teleport
        Teleport();

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

        MoveFruit(best_move); // do the move
        wait = Globals::fDelay; // reset the delay to slow down the fruit by three ticks
        return 0;
    }
    return 0; // clean exit
}

void Fruit::CoutFruit()
{
    switch (fruit_type)
    {
    case Fruits::CHERRY:
        Draw::SetColor(Globals::cCHERRY);
        cout << char(Globals::fCherry);
        break;
    case Fruits::STRAWBERRY:
        Draw::SetColor(Globals::cSTRAWBERRY);
        cout << char(Globals::fStrawberry);
        break;
    case Fruits::APPLE:
        Draw::SetColor(Globals::cAPPLE);
        cout << char(Globals::fApple);
        break;
    case Fruits::PEAR:
        Draw::SetColor(Globals::cPEAR);
        cout << char(Globals::fPear);
        break;
    default:
        Draw::SetColor(Globals::cCHERRY);
        cout << char(Globals::fCherry);
    }
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

void Fruit::SetFruitEaten()
{
    not_eaten = false;
}
