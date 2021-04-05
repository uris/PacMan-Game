#include "Game.h"
#include <iomanip> // abs function
#include <iostream>

using namespace std;
using namespace std::chrono;

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
        chase_modifier = { 0, 0 };
        this->ghost = 'R';
        color = 71;
        wait = 15;
        break;
    case Ghosts::YELLOW:
        name = Ghosts::YELLOW;
        chase_modifier = { 0, 3 };
        this->ghost = 'Y';
        color = 367;
        wait = 30;
        break;
    case Ghosts::BLUE:
        name = Ghosts::BLUE;
        chase_modifier = { 0, -3 };
        this->ghost = 'B';
        color = 435;
        wait = 45;
        break;
    case Ghosts::PINK:
        name = Ghosts::PINK;
        chase_modifier = { -3, 0 };
        this->ghost = 'P';
        color = 479;
        wait = 60;
        break;
    default:
        name = Ghosts::RED;
        chase_modifier = { 0, 0 };
        name = Ghosts::RED;
        this->ghost = 'R';
        color = 71;
        wait = 15;
        break;
    }
}

// methods
//int Ghost::DistanceToPlayer(Coord ghost_position, Coord player_current_position)
//{
//    return (abs(ghost_position.col - (player_current_position.col + chase_modifier.col)) + abs(ghost_position.row - (player_current_position.row + chase_modifier.row)));
//}
//
//int Ghost::DistanceToRoamTarget(Coord ghost_position)
//{
//    // overload to return the distance to a coord with a specified modifier
//    return ( abs(ghost_position.col - roam_target.col) + abs(ghost_position.row - roam_target.row) );
//}
//
//int Ghost::DistanceToSpawnTarget(Coord ghost_position)
//{
//    // overload to return the distance to a coord with a specified modifier
//    return (abs(ghost_position.col - spawn_target.col) + abs(ghost_position.row - spawn_target.row));
//}

bool Ghost::PlayerCollision()
{
    return current_position == p_game->p_player->GetCurrentPosition();
        
}

int Ghost::MakeGhostMove()
{
    Direction best_move = Direction::NONE; // will store the next move
    
    if (skip_turn || p_game->IsGameOver() || PlayerCollision())
    {
        return 0; // next ghost (no move)
    }

    if (wait > 0)
    {
        wait--; // ghost is waiting to leave spawn area
        return 0; // next ghost (no move)
    }

    if (mode == Mode::RUN) // make random moves truning opposite direction on first move
    {
        best_move = RandomGhostMove(); // get the random move
        char map_content = GhostContentNow(best_move);
        MoveGhost(best_move, map_content); // make the move
        return 0;
    }

    if (mode != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
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
                // it's a viable move to position so set new coords of ghost
                prior_position = current_position;
                current_position = next_move;

                // calculate distance score based on new coords
                int score = GetBestMove(next_move, new_direction, 1); // get the minimax score for the move (recurssive)

                // revert ghost coords back
                current_position = prior_position;

                // if the new move score gets the ghost closer to the target coord
                if (score < best_score)
                {
                    best_score = score; // set new best score
                    best_move = new_direction; // set new best move direction
                }
            }
        }

        char map_content = GhostContentNow(best_move); // if the content is a ghosts set the contnet value of the gohst to the other ghosts content
        MoveGhost(best_move, map_content); // do the move
        return 0;
    }
    return 0; // clean exit
}

int Ghost::GetBestMove(Coord current_position, Direction current_direction, int depth)
{
    Coord player_position = p_game->p_player->GetCurrentPosition();
    
    //if on teleport do the teleport
    Teleport(current_position, current_direction);
    
    // and on the target the ghost chases: red chases player pos, yellow player pos + 2 cols (to the right of player)
    switch (mode)
    {
    case Mode::CHASE: // redude distance to player
        if (current_position % (player_position + chase_modifier) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == Globals::look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return ((current_position % player_position) + depth); // add depth to get fastest path
        }
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == Globals::look_ahead) {
            return current_position % roam_target;
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        if (current_position % spawn_target == 0)
            mode = (p_game->p_level->level_mode == Mode::RUN ? Mode::CHASE : p_game->p_level->level_mode);
        return current_position % spawn_target;
        break;
    }

    // if its the ghost move we want the lowest possible distance to player
    int score = 1000, best_score = 1000;
    Coord next_move, prior_position;
    Direction new_direction = Direction::NONE;
    
    for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
    {
        new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
        next_move.SetTo(current_position, new_direction);

        if (p_game->p_level->NotWall(next_move, new_direction) && !IsReverseDirection(new_direction)) // check if the direction is valid i.e not wall or reverse
        {
            // set new coords of ghost
            prior_position = current_position;
            current_position = next_move;

            // calculate distance score based on new coords
            int score = GetBestMove(next_move, new_direction, depth + 1); // get the minimax score for the move (recurssive)

            // revert ghost coords back
            current_position = prior_position;

            // if the score of is better than the current bestscore set the score to be the new best score (min to get closer, max when running away)
            best_score = min(score, best_score);
        }
    }
    return best_score;
}

void Ghost::Teleport(Coord& ghost_position, Direction& ghost_direction)
{
    if (p_game->p_level->IsTeleport(ghost_position))
    {
        if (ghost_position.col == 0 && ghost_direction == Direction::LEFT)
        {
            ghost_position.col = p_game->p_level->cols - 1;
        }
        if (ghost_position.col == p_game->p_level->cols - 1 && ghost_direction == Direction::RIGHT)
        {
            ghost_position.col = 0;
        }
    }
}

Direction Ghost::RandomGhostMove()
{
    Direction new_direction = Direction::NONE;
    Coord next_move;

    // first move when on the run is ALWAYS to reverse direction which is ALWAYS a valid move
    if (run_first_move) {
        switch (current_direction)
        {
        case Direction::UP:
            new_direction = Direction::DOWN;
            break;
        case Direction::RIGHT:
            new_direction = Direction::LEFT;
            break;
        case Direction::DOWN:
            new_direction = Direction::UP;
            break;
        case Direction::LEFT:
            new_direction = Direction::RIGHT;
            break;
        }
        run_first_move = false;
        return new_direction;
    }

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

char Ghost::GhostContentNow(Direction best_move)
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

void Ghost::MoveGhost(const Direction direction, const char map_content)
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
    PlayerCollision() ? square_content_now = ' ' : square_content_now;

}

bool Ghost::GameRefIsSet()
{
    return (p_game ? true : false);
}

// setters
void Ghost::SetGameRef(Game* p_game)
{
    this->p_game = p_game;
    switch (name)
    {
    case Ghosts::RED:
        roam_target = { p_game->p_level->rows + 3, p_game->p_level->cols - 3 };
        break;
    case Ghosts::YELLOW:
        roam_target = { p_game->p_level->rows + 3, 3 };
        break;
    case Ghosts::BLUE:
        roam_target = { -3, 3 };
        break;
    case Ghosts::PINK:
        roam_target = { -3 , p_game->p_level->cols - 3 };
        break;
    default:
        roam_target = { p_game->p_level->rows + 3, p_game->p_level->cols - 2 };
        break;
    }
}

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

void Ghost::SetContentCurrent(bool same)
{
    square_content_now = square_content_now;
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

void Ghost::SpawnGhost(Ghosts name, bool player_died)
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
