#include "Ghost.h"
#include "EnumsAndStatics.h"
#include <iomanip> // abs function
#include <iostream>
#include <chrono>

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


// methods
int Ghost::DistanceToPlayer(Coord player_current_position)
{
    // overload to return the distance to a coord with a specified modifier
    return (abs(current_position.col - (player_current_position.col + chase_modifier.col)) + abs(current_position.row - (player_current_position.row + chase_modifier.row)));
}

int Ghost::DistanceToRoamTarget()
{
    // overload to return the distance to a coord with a specified modifier
    return ( abs(current_position.col - roam_target.col) + abs(current_position.row - roam_target.row) );
}

int Ghost::DistanceToSpawnTarget()
{
    // overload to return the distance to a coord with a specified modifier
    return (abs(current_position.col - spawn_target.col) + abs(current_position.row - spawn_target.row));
}

bool Ghost::PlayerCollision(Coord player_coord)
{
    return (current_position.row == player_coord.row && current_position.col == player_coord.col) ? true : false;
        
}

int Ghost::GetGhostMove(const bool game_over, Player* p_player, char** p_map, Mode level_mode, Ghost** p_ghosts)
{
    Direction best_move = Direction::NONE; // will store the next move
    char map_content = ' '; // will store the content of the map at the move position

  
        if (skip_turn || game_over || PlayerCollision(p_player->GetCurrentPosition()))
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
            best_move = RandomGhostMove(p_map); // get the random move
            map_content = GetMapContent(p_map, current_position, best_move); // get map content at the move position
            MoveGhost(p_player->GetCurrentPosition(), best_move, map_content); // make the move
            return 0; // next ghost
        }

        if (mode != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
        {
            int score = 0, best_score = 1000;
            Coord next_move, prior_position;
            Direction new_direction = Direction::NONE;

            for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
            {

                new_direction = static_cast<Direction>(i); // set the direction we will get best move for
                map_content = GetMapContent(p_map, current_position, new_direction); // get the content on the map pos we are moving to 

                if (NotWall(map_content, new_direction) && !IsReverseDirection(new_direction)) // check for next available square
                {
                    // it's a viable move to position so set new coords of ghost
                    next_move.SetTo(current_position, new_direction);
                    prior_position = current_position;
                    current_position = next_move;

                    // calculate distance score based on new coords
                    //int score = GetBestMove(g, next_move, new_direction, 1); // get the minimax score for the move (recurssive)

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

            map_content = GetMapContent(p_map, current_position, best_move); // get map content at the move position
            map_content = GhostContentNow(p_ghosts, p_player, map_content); // if the content is a ghosts set the contnet value of the gohst to the other ghosts content

           MoveGhost(p_player->GetCurrentPosition(), best_move, map_content); // do the move

            return 0; // clean exit
        }
    

}

int Ghost::GetBestMove(char** p_map, Player* p_player, Coord current_position, Direction current_direction, Mode level_mode, int depth)
{
    // and on the target the ghost chases: red chases player pos, yellow player pos + 2 cols (to the right of player)
    switch (mode)
    {
    case Mode::CHASE: // redude distance to player
        if (DistanceToPlayer(p_player->GetCurrentPosition()) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == Globals::look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return (DistanceToPlayer(p_player->GetCurrentPosition()) + depth); // add depth to get fastest path
        }
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == Globals::look_ahead) {
            return DistanceToRoamTarget();
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        if (DistanceToSpawnTarget() == 0)
            mode = (level_mode == Mode::RUN ? Mode::CHASE : level_mode);
        return DistanceToSpawnTarget();
        break;
    }

    // if its the ghost move we want the lowest possible distance to player
    int score = 1000, best_score = 1000;
    Coord next_move, prior_position;
    Direction new_direction = Direction::NONE;
    char map_content = ' '; // will store the content of the map at the move position

    for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
    {
        new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
        map_content = GetMapContent(p_map, current_position, new_direction); // get the content on the map pos we are moving to 

        if (NotWall(map_content, new_direction) && !IsReverseDirection(new_direction)) // check if the direction is valid i.e not wall or reverse
        {
            // set new coords of ghost
            next_move.SetTo(current_position, new_direction);
            prior_position = current_position;
            current_position = next_move;

            // calculate distance score based on new coords
            int score = GetBestMove(p_map, p_player, next_move, new_direction, level_mode, depth + 1); // get the minimax score for the move (recurssive)

            // revert ghost coords back
            current_position = prior_position;

            // if the score of is better than the current bestscore set the score to be the new best score (min to get closer, max when running away)
            best_score = min(score, best_score);
        }
    }
    return best_score;
}

Direction Ghost::RandomGhostMove(char** p_map)
{
    Direction newDirection = Direction::NONE;

    // first move when on the run is ALWAYS to reverse direction which is ALWAYS a valid move
    if (run_first_move) {
        switch (current_direction)
        {
        case Direction::UP:
            newDirection = Direction::DOWN;
            break;
        case Direction::RIGHT:
            newDirection = Direction::LEFT;
            break;
        case Direction::DOWN:
            newDirection = Direction::UP;
            break;
        case Direction::LEFT:
            newDirection = Direction::RIGHT;
            break;
        }
        run_first_move = false;
        return newDirection;
    }

    Coord move = current_position;
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (IsTeleport(p_map)) // if on teleportkeep the same direction
        current_direction == Direction::LEFT ? newDirection = Direction::LEFT : newDirection = Direction::RIGHT;
    else // else choose a valid random direction
    {
        do
        {
            int random_number = rand() % 4; //generate random number to select from the 4 possible options
            newDirection = static_cast<Direction>(random_number);
        } while (NotWall(GetMapContent(p_map, current_position, newDirection), newDirection) || IsReverseDirection(newDirection));
    }
    return newDirection;
}

char Ghost::GhostContentNow(Ghost** p_ghosts, Player* p_player, char map_content)
{
    switch (map_content)
    {
    case Globals::red_ghost:
        return p_ghosts[0]->GetContentCurrent();
        break;
    case Globals::yellow_ghost:
        return p_ghosts[1]->GetContentCurrent();
        break;
    case Globals::blue_ghost:
        return p_ghosts[2]->GetContentCurrent();
        break;
    case Globals::pink_ghost:
        return p_ghosts[3]->GetContentCurrent();
        break;
    case Globals::player:
        return p_player->GetMovedIntoSquareContents();
        break;
    default:
        return map_content;
        break;
    }
}

char Ghost::GetMapContent(char** p_map, Coord map_coord, Direction direction)
{
    switch (direction)
    {
    case Direction::UP:
        return p_map[map_coord.row - 1][map_coord.col];
        break;
    case Direction::RIGHT:
        return p_map[map_coord.row][map_coord.col + 1];
        break;
    case Direction::DOWN:
        return p_map[map_coord.row + 1][map_coord.col];
        break;
    case Direction::LEFT:
        return p_map[map_coord.row][map_coord.col - 1];
        break;
    default:
        return ' ';
        break;
    }
}

bool Ghost::IsTeleport(char** p_map)
{
    return (p_map[current_position.row][current_position.col] == Globals::teleport ? true : false);
}

void Ghost::MoveGhost(const Coord player_coord, const Direction direction, const char map_content)
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
