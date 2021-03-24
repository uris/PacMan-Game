#include "Game.h"
#include "Draw.h"
#include "Utility.h"
#include <conio.h>
#include <iostream>
#include <Windows.h> // colors and play sounds
#include <mmsystem.h> // play .wav
#include <thread> // used to get input while running game
#include <iomanip> // formating output

using namespace std;

//constructors
Game::Game() {};

//destructors
Game::~Game()
{
    // delete ghosts array and set pointer to null
    if (p_ghosts) {
        for (int i = 0; i < Globals::total_ghosts; i++)
        {
            delete[] p_ghosts[i];
        }
        delete[] p_ghosts;
        p_ghosts = nullptr;
    }
    
    // player pointer to null
    if (p_player) {
        delete p_player;
        p_player = nullptr;
    }

    // level pointer to null
    if (p_level) {
        delete p_level;
        p_level = nullptr;
    }
};
 
//game flow
void Game::RunGame()
{
    GameCredits(); // show game credits

    do
    {
        // Set up game and initialize data
        SetupGame();

        // Set up game and initialize data
        DrawLevel();

        // start a thread to get input while the main program executes
        thread inputThread(&Game::GetKeyboardInput, this); // new the ref() wrapper to pass by ref into thread

        do
        {
            // move the player
            MovePlayer();

            // process any player / ghost collision
            PlayerMonsterCollision();

            // set ghost chase modes as approprite
            SetGhostMode();

            ////// move the ghost
            MoveGhosts();

            // process any player / ghost collision again
            PlayerMonsterCollision();

            // delay render if there's a collision
            SetCollisionDelay();

            // Draw the level current state
            DrawLevel();

            // end condition for the gmae once user has eaten all pellets
            CheckLevelComplete();

            // display stats and number lives
            PrintStatusBar();

            // introduce a wait for fast PC
            SetRefreshDelay();

        } while (!p_level->is_complete && !game_over);

        // kill any sounds playing
        SFX(Play::NONE);

        // when game ends, wait for the input thread rejoin the main thread
        inputThread.join();

        // continue to next level, play again or quit
        if (!NextLevelRestartGame())
            break;

    } while (true);

}

void Game::GameCredits()
{
    Draw draw;
    Utility utility;
    string ghost;

    draw.ShowConsoleCursor(false);

    ghost += "               *********\n";
    ghost += "           *****************\n";
    ghost += "         *********************\n";
    ghost += "       ******####********####**\n";
    ghost += "       ****#########***#########\n";
    ghost += "       ****#####    ***#####    *\n";
    ghost += "    *******#####    ***#####    ***\n";
    ghost += "    *********#####********####*****\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *******************************\n";
    ghost += "    *****  *******    ******  *****\n";
    ghost += "    ***      *****    ****      ***\n";

    // print image
    string format = utility.Spacer("PACMAN 2021", 40);
    cout << endl << endl;
    draw.SetColor(7);
    cout << ghost;
    draw.SetColor(7);
    cout << endl;
    cout << format;
    cout << "PACMAN 2021";
    cout << format;

    // play intro
    SFX(Play::INTRO);

    // any key press starts game
    char input = _getch();
    system("cls");
}

void Game::SetupGame()
{
    // if restarting the game then reset player lives and game over state
    game_over ? p_player->SetLives(3) : p_player->SetLives();
    p_player->ClearEatenGohsts(); // clear the ghosts eaten in a row
    game_over = false;

    // reset ghosts to spawn state
    SpawnAllGhosts();

    // reset level, create scene and update player / level as needed
    p_level->SetupLevel(p_player, p_ghosts, current_scene);
}

void Game::DrawLevel()
{
    p_level->DrawLevel(p_player, p_ghosts); //draw level

    // pause before starting the level
    if (p_level->level_paused) {
        // before start of level get any key to start
        string format = utility.Spacer("Press any key to start.", p_level->cols);
        cout << endl;
        cout << format;
        cout << "Press any key to start.";
        cout << format;
        SFX(Play::INTERMISSION);
        char input = _getch();
        p_level->level_paused = false;
    }
    else
    {
        // level is playing and we can play the appropriate game sound
        switch (p_player->GetMovedIntoSquareContents())
        {
        case (char)Globals::pellet:
            SFX(Play::MUNCH);
            break;
        case (char)Globals::powerup:
            SFX(Play::POWER_UP);
            break;
        case Globals::space:
        case Globals::player:
            SFX(Play::SIREN);
            break;
        }
    }
}

void Game::MovePlayer()
{
    // scan content one sqaure around the player and store chars in an array
    SetMapContents(p_player->GetCurrentPosition());

    // move the player based on the content of the move to sqaure
    p_player->MovePlayer(map_contents);

    // set any statuses that need setting
    SetPlayerState();

    // check collision with a ghost and set the ghosts previous row to the new player position
    for (int g = 0; g < Globals::total_ghosts; g++)
    {
        if (PlayerGhostCollision(g)) {
            p_ghosts[g]->GetPreviousPosition().SetTo(p_player->GetCurrentPosition());
        }
    }
}

int Game::MoveGhosts()
{
    //p_ghosts[0]->GetGhostMove(game_over, p_player, p_level->p_map, p_level->level_mode, p_ghosts);
    
    Direction best_move = Direction::NONE; // will store the next move
    char map_content = ' '; // will store the content of the map at the move position

    for (int g = 0; g < Globals::total_ghosts; g++)
    {
        if (p_ghosts[g]->SkipTurn() || game_over || PlayerGhostCollision(g))
        {
            continue; // next ghost (no move)
        }

        if (p_ghosts[g]->GetWait() > 0)
        {
            p_ghosts[g]->DecreaseWait(); // ghost is waiting to leave spawn area
            continue; // next ghost (no move)
        }

        if (p_ghosts[g]->GetMode() == Mode::RUN) // make random moves truning opposite direction on first move
        {
            best_move = RandomGhostMove(g); // get the random move
            map_content = GetMapContent(p_ghosts[g]->GetCurrentPosition(), best_move); // get map content at the move position
            p_ghosts[g]->MoveGhost(p_player->GetCurrentPosition(), best_move, map_content); // make the move
            continue; // next ghost
        }

        if (p_ghosts[g]->GetMode() != Mode::RUN)  // run recursive AI for chase, roam and spawn modes 
        {
            int score = 0, best_score = 1000;
            Coord next_move;
            Coord current_posiiton = p_ghosts[g]->GetCurrentPosition();
            Direction new_direction = Direction::NONE;

            for (int i = 0; i <= 3; i++) // cycle through up,down,left,right to find the valid best next move
            {

                new_direction = static_cast<Direction>(i); // set the direction we will get best move for
                map_content = GetMapContent(current_posiiton, new_direction); // get the content on the map pos we are moving to 

                if (p_ghosts[g]->NotWall(map_content, new_direction) && !p_ghosts[g]->IsReverseDirection(new_direction)) // check for next available square
                {
                    // it's a viable move to position so set new coords of ghost
                    next_move.SetTo(current_posiiton, new_direction);
                    p_ghosts[g]->SetCurrentPosition(next_move);

                    // calculate distance score based on new coords
                    int score = GetBestMove(g, next_move, new_direction, 1); // get the minimax score for the move (recurssive)

                    // revert ghost coords back
                    p_ghosts[g]->SetCurrentPosition(current_posiiton);

                    // if the new move score gets the ghost closer to the target coord
                    if (score < best_score)
                    {
                        best_score = score; // set new best score
                        best_move = new_direction; // set new best move direction
                    }
                }
            }

            map_content = GetMapContent(p_ghosts[g]->GetCurrentPosition(), best_move); // get map content at the move position
            map_content = GhostContentNow(map_content); // if the content is a ghosts set the contnet value of the gohst to the other ghosts content
            
            p_ghosts[g]->MoveGhost(p_player->GetCurrentPosition(), best_move, map_content); // do the move

            continue; // next ghost
        }
    }

    return 0; // clean exit
}

char Game::GhostContentNow(char map_content)
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

void Game::PrintStatusBar()
{
    //Draw draw;
    //Utility utility;
    // get number of lives
    string format = utility.Spacer("LIVES:CCC    SCORE:00000000", p_level->cols);
    string lives;
    for (int i = 0; i < 6; i++)
    {
        i < p_player->Lives() ? lives = lives + Globals::player : lives = lives +  " ";
    }
    p_player->SetScore(((p_level->eaten_pellets + 1) * p_level->points_pellet) + (p_level->eaten_ghosts * p_level->points_ghost) + (p_level->eaten_ghosts >= 4 ? p_level->all_ghost_bonus : 0));
    cout << endl;
    cout << format;
    draw.SetColor(7);
    cout << "LIVES:";
    draw.SetColor(14);
    cout << (!p_player->HasNoLives() ? lives : "-");
    draw.SetColor(7);
    cout << " SCORE:";
    draw.SetColor(14);
    cout << setfill('0') << setw(8) << p_player->GetScore();
    draw.SetColor(7);
    cout << format;
    cout << "\r";

    // message game over or level complete
    if (game_over)
    {
        format = utility.Spacer("Game Over! Press a key to continue.", p_level->cols);
        cout << format;
        cout << "Game Over! Press a key to continue.";
        cout << format;
    }
    else if (p_level->is_complete)
    {
        format = utility.Spacer("Level complete! Press a key to continue.", p_level->cols);
        cout << format;
        cout << "Level complete! Press a key to continue.";
        cout << format;
    }

}

void Game::SetGhostMode()
{
    switch (p_level->level_mode)
    {

        // set chase mode stuff
    case Mode::CHASE:
        p_level->chase_duration = duration_cast<duration<double>>(p_level->chase_time_start - chrono::high_resolution_clock::now());
        if (abs(p_level->chase_duration.count()) > p_level->chase_for && p_level->roam_count > 0)
        {
            p_level->level_mode = Mode::ROAM;
            p_level->roam_time_start = chrono::high_resolution_clock::now();
            p_level->roam_count--;

            for (int g = 0; g < Globals::total_ghosts; g++) {
                switch (p_ghosts[g]->GetMode())
                {
                case Mode::ROAM:
                case Mode::CHASE:
                    p_ghosts[g]->SetMode(Mode::ROAM);
                    p_ghosts[g]->SetSkipTurn(false);
                    break;
                case Mode::RUN:
                case Mode::SPAWN:
                    p_ghosts[g]->SetMode(p_ghosts[g]->GetMode());
                    break;
                }
            }
        }
        break;

        // set roam mode stuff
    case Mode::ROAM:
        p_level->roam_duration = duration_cast<duration<double>>(p_level->roam_time_start - chrono::high_resolution_clock::now());

        if (abs(p_level->roam_duration.count()) > p_level->roam_for)
        {
            p_level->level_mode = Mode::CHASE;
            p_level->chase_time_start = chrono::high_resolution_clock::now();

            for (int g = 0; g < Globals::total_ghosts; g++) {
                switch (p_ghosts[g]->GetMode())
                {
                case Mode::ROAM:
                    p_ghosts[g]->SetMode(Mode::CHASE);
                    break;
                case Mode::CHASE:
                case Mode::RUN:
                case Mode::SPAWN:
                    p_ghosts[g]->SetMode(p_ghosts[g]->GetMode());
                }
            }
        }
        break;

        // set run mode stuff
    case Mode::SPAWN:
        for (int g = 0; g < Globals::total_ghosts; g++)
        {
            if (p_ghosts[g]->GetCurrentPosition().IsSame(p_level->ghost_spawn))
            {
                p_ghosts[g]->SetMode(Mode::CHASE);
            }
        }
        break;

        // set run mode stuff
    case Mode::RUN:
        p_level->run_duration = duration_cast<duration<double>>(p_level->run_time_start - chrono::high_resolution_clock::now());
        if (abs(p_level->run_duration.count()) > p_level->run_for) // end the run mode
        {
            p_level->level_mode = Mode::CHASE;
            p_level->chase_time_start = chrono::high_resolution_clock::now();

            for (int g = 0; g < Globals::total_ghosts; g++) {
                switch (p_ghosts[g]->GetMode())
                {
                case Mode::RUN:
                case Mode::ROAM:
                    p_ghosts[g]->SetMode(Mode::CHASE);
                    break;
                case Mode::CHASE:
                case Mode::SPAWN:
                    p_ghosts[g]->SetMode(p_ghosts[g]->GetMode());
                }
                p_ghosts[g]->SetSkipTurn(false); // when ghost is edible slow him down
                p_ghosts[g]->SetEdible(false);
                p_ghosts[g]->SetReverseMove(true);
            }
        }
        else // set the ghosts states while in run mode - some ghots can be in run mode and others in chase mode
        {
            for (int g = 0; g < Globals::total_ghosts; g++) {
                if (p_ghosts[g]->GetMode() == Mode::RUN)
                    p_ghosts[g]->SetSkipTurn(!p_ghosts[g]->SkipTurn()); // when ghost is edible slow him down
                else
                    p_ghosts[g]->SetReverseMove(false);
            }

        }
        break;
    }


}


// game orchestration methods
void Game::Add(Ghost* red, Ghost* yellow, Ghost* blue, Ghost* pink)
{
    p_ghosts = new Ghost*[4]{ red, yellow, blue, pink };

}

void Game::Add(Player* player)
{
    p_player = player;
}

void Game::Add(Level* level)
{
    p_level = level;
}

void Game::SpawnThisGhost(Ghosts name, bool player_died)
{
    if (p_ghosts)
    {
        switch (name)
        {
        case Ghosts::RED:
            p_ghosts[0]->SpawnGhost(player_died);
            break;
        case Ghosts::YELLOW:
            p_ghosts[1]->SpawnGhost(player_died);
            break;
        case Ghosts::BLUE:
            p_ghosts[2]->SpawnGhost(player_died);
            break;
        case Ghosts::PINK:
            p_ghosts[3]->SpawnGhost(player_died);
            break;
        default:
            break;
        }
    }

}

void Game::SpawnAllGhosts()
{
    if (p_ghosts)
    {
        for (int i = 0; i < Globals::total_ghosts; i++)
        {
            p_ghosts[i]->SpawnGhost(true);
        }
    }

}

void Game::GetKeyboardInput()
{
    int input;

    do
    {
        input = _getch();

        //if special character then get special key into input
        (input && input == 224) ? input = _getch() : input;

        switch (input)
        {
        case Globals::kARROW_LEFT:
        case Globals::kLEFT:
            p_player->SetDirection(Direction::LEFT);
            break;
        case Globals::kARROW_RIGHT:
        case Globals::kRIGHT:
            p_player->SetDirection(Direction::RIGHT);
            break;
        case Globals::kARROW_UP:
        case Globals::kUP:
            p_player->SetDirection(Direction::UP);
            break;
        case Globals::kARROW_DOWN:
        case Globals::kDOWN:
            p_player->SetDirection(Direction::DOWN);
            break;
        default:
            break;
        }
    } while (!p_level->is_complete && !game_over);
}

void Game::NextScene()
{
    current_scene++;
}

int Game::SFX(Play playSFX)
{
    LPCTSTR t_sfx = NULL;
    bool played = false;
    bool timerOn = false;
    DWORD params = SND_FILENAME | SND_ASYNC;
    float duration = 0.0f;

    switch (playSFX)
    {
    case Play::INTRO:
        t_sfx = TEXT("sfx_intro.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::INTERMISSION:
        t_sfx = TEXT("sfx_intermission.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::MUNCH:
        if (sfx == Play::MUNCH)
            return 0;
        t_sfx = TEXT("sfx_munch_lg.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::DEATH:
        t_sfx = TEXT("sfx_death.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::CREDIT:
        t_sfx = TEXT("sfx_bonus.wav");
        break;
    case Play::POWER_UP:
        t_sfx = TEXT("sfx_powerup.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::SIREN:
        if (sfx == Play::SIREN)
            return 0;
        t_sfx = TEXT("sfx_siren.wav");
        params = SND_LOOP | SND_FILENAME | SND_ASYNC;
        break;
    case Play::LIFE:
        t_sfx = TEXT("sfx_xlife.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::EAT_GHOST:
        t_sfx = TEXT("sfx_eatghost.wav");
        params = SND_FILENAME | SND_SYNC;
        break;
    case Play::EAT_FRUIT:
        t_sfx = TEXT("sfx_eatfruit.wav");
        break;
    default:
        t_sfx = NULL;
        break;
    }

    // immediately cancel all audio sfx is trying to play none
    if (playSFX == Play::NONE)
    {
        played = PlaySound(NULL, NULL, params);
        sfx = Play::NONE;
        return 0;
    }

    // if mode is RUN and not ghost eat or death, then play power up
    if (p_level->level_mode == Mode::RUN)
    {
        if (sfx != Play::POWER_UP)
        {
            played = PlaySound(TEXT("sfx_powerup.wav"), NULL, SND_LOOP | SND_FILENAME | SND_ASYNC);
            sfx = Play::POWER_UP;
            return 0;
        }
    }

    if (playSFX != sfx)
    {
        if (playSFX == Play::DEATH || playSFX == Play::EAT_GHOST || playSFX == Play::LIFE)
        {
            played = PlaySound(t_sfx, NULL, params);
            sfx = playSFX;
            return 0;
        }
        else if (p_level->level_mode != Mode::RUN)
        {
            played = PlaySound(t_sfx, NULL, params);
            sfx = playSFX;
            return 0;
        }
    }

    return 0;

}

char Game::GetMapContent(Coord map_coord, Direction direction)
{
    switch (direction)
    {
    case Direction::UP:
        return p_level->p_map[map_coord.row - 1][map_coord.col];
        break;
    case Direction::RIGHT:
        return p_level->p_map[map_coord.row][map_coord.col + 1];
        break;
    case Direction::DOWN:
        return p_level->p_map[map_coord.row + 1][map_coord.col];
        break;
    case Direction::LEFT:
        return p_level->p_map[map_coord.row][map_coord.col - 1];
        break;
    default:
        return ' ';
        break;
    }
}

void Game::SetMapContents(Coord map_coord)
{
    map_contents[0] = p_level->p_map[map_coord.row - 1][map_coord.col]; // up
    map_contents[1] = p_level->p_map[map_coord.row][map_coord.col + 1]; // right
    map_contents[2] = p_level->p_map[map_coord.row + 1][map_coord.col]; // down
    map_contents[3] = p_level->p_map[map_coord.row][map_coord.col - 1]; // left
}

bool Game::PlayerGhostCollision(const int g)
{
    return p_player->GetCurrentPosition().IsSame(p_ghosts[g]->GetCurrentPosition()) ? true : false;
}

void Game::PlayerMonsterCollision()
{
    bool player_died = false;

    for (int g = 0; g < Globals::total_ghosts; g++) // loop ghosts
    {
        if (p_player->GetCurrentPosition().IsSame(p_ghosts[g]->GetCurrentPosition()))
        {
            DrawLevel(); // print the move immediately
            if (p_ghosts[g]->IsEdible()) // ghost dies
            {
                // if the ghost was on a pellet/powerup sqaure need to count it up
                if (p_ghosts[g]->GetContentCurrent() == (char)Globals::pellet || p_ghosts[g]->GetContentCurrent() == (char)Globals::powerup)
                    p_level->eaten_pellets++;
                p_level->eaten_ghosts++; // increment ghosts eaten
                gobble_pause = true; // set small pause after eating ghost
                SpawnThisGhost(p_ghosts[g]->Name(), false);
                p_player->EatGhost(g);
                if (p_player->AllGhostsEaten()) {
                    p_level->all_eaten_ghosts += 1;
                    SFX(Play::LIFE);
                    p_player->ClearEatenGohsts();
                    p_level->level_mode = Mode::CHASE;
                    p_level->chase_time_start = chrono::high_resolution_clock::now();
                }
                else {
                    SFX(Play::EAT_GHOST);
                }

            }
            else
            {
                // player dies
                player_died = true;
            }
        }
    }

    if (player_died) {

        // rest the roam count by adding one back to the count
        p_level->level_mode == Mode::ROAM ? p_level->roam_count++ : p_level->roam_count;

        // set the level mode to chase
        p_level->level_mode = Mode::CHASE;
        p_level->chase_time_start = chrono::high_resolution_clock::now();

        // end game or respawn player if no lives left
        p_player->TakeLives(1);
        if (!p_player->HasNoLives())
        {
            player_beat_pause = true; // give player time to get ready
            p_player->ReSpawn();
            for (int g = 0; g < Globals::total_ghosts; g++) {
                // replace ghost with empty sqaure since the player was there an any pellet got eaten
                p_level->p_map[p_ghosts[g]->GetCurrentRow()][p_ghosts[g]->GetCurrentCol()] = ' ';
                // respawn ghost
                SpawnAllGhosts();
            }
            SFX(Play::DEATH);
        }
        else
        {
            game_over = true;
            SFX(Play::DEATH);
        }
    }
}

int Game::GetBestMove(int g, Coord current_position, Direction current_direction, int depth)
{
    // and on the target the ghost chases: red chases player pos, yellow player pos + 2 cols (to the right of player)
    switch (p_ghosts[g]->GetMode())
    {
    case Mode::CHASE: // redude distance to player
        if (p_ghosts[g]->DistanceToPlayer(p_player->GetCurrentPosition()) == 0) { // if player and ghost collide return 0 + depth as score
            return 0 + depth; // add depth to get fastest path
        }
        if (depth == Globals::look_ahead) { // if depth X return the distance score, increase this to make the ghost look forward more
            return (p_ghosts[g]->DistanceToPlayer(p_player->GetCurrentPosition()) + depth); // add depth to get fastest path
        }
        break;
    case Mode::ROAM: // reduce distance to the ghost's roam target
        if (depth == Globals::look_ahead) {
            return p_ghosts[g]->DistanceToRoamTarget();
        }
        break;
    case Mode::SPAWN: // target is above the exit area
        if (p_ghosts[g]->DistanceToSpawnTarget() == 0)
            p_ghosts[g]->SetMode(p_level->level_mode == Mode::RUN ? Mode::CHASE : p_level->level_mode);
        return p_ghosts[g]->DistanceToSpawnTarget();
        break;
    }

    // if its the ghost move we want the lowest possible distance to player
    int score = 1000, best_score = 1000;
    Coord next_move;
    Direction new_direction = Direction::NONE;
    char map_content = ' '; // will store the content of the map at the move position

    for (int i = 0; i <= 3; i++) // cycle through each next possible move up, down, left, right
    {
        new_direction = static_cast<Direction>(i); // cast index to direction up, down, left, right
        map_content = GetMapContent(current_position, new_direction); // get the content on the map pos we are moving to 

        if (p_ghosts[g]->NotWall(map_content, new_direction) && !p_ghosts[g]->IsReverseDirection(new_direction)) // check if the direction is valid i.e not wall or reverse
        {
            // set new coords of ghost
            next_move.SetTo(current_position, new_direction);
            p_ghosts[g]->SetCurrentPosition(next_move);

            // calculate distance score based on new coords
            int score = GetBestMove(g, next_move, new_direction, depth + 1); // get the minimax score for the move (recurssive)

            // revert ghost coords back
            p_ghosts[g]->SetCurrentPosition(current_position);

            // if the score of is better than the current bestscore set the score to be the new best score (min to get closer, max when running away)
            best_score = min(score, best_score);
        }
    }
    return best_score;
}

Direction Game::RandomGhostMove(int g)
{
    Direction newDirection = Direction::NONE;

    // first move when on the run is ALWAYS to reverse direction which is ALWAYS a valid move
    if (p_ghosts[g]->ReverseMove()) {
        switch (p_ghosts[g]->GetDirection())
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
        p_ghosts[g]->SetReverseMove(false);
        return newDirection;
    }

    Coord move(p_ghosts[g]->GetCurrentPosition());
    int unsigned seed = (int)(std::chrono::system_clock::now().time_since_epoch().count());
    srand(seed);

    if (p_level->IsTeleport(move)) // if on teleportkeep the same direction
        p_ghosts[g]->GetDirection() == Direction::LEFT ? newDirection = Direction::LEFT : newDirection = Direction::RIGHT;
    else // else choose a valid random direction
    {
        do
        {
            int randomNumber = rand() % 4; //generate random number to select from the 4 possible options
            newDirection = static_cast<Direction>(randomNumber);
        } while (!p_level->NotWall(p_player, Coord(move, newDirection), newDirection) || p_ghosts[g]->IsReverseDirection(newDirection));
    }
    return newDirection;
}

void Game::SetPlayerState()
{
    char levelObj = p_level->p_map[p_player->GetCurrentRow()][p_player->GetCurrentCol()];
    switch (levelObj)
    {
    case (char)Globals::pellet: // eat pellet
        p_level->eaten_pellets++;
        break;
    case (char)Globals::powerup: // eat power up
        for (int g = 0; g < Globals::total_ghosts; g++) // loop ghosts
        {
            p_ghosts[g]->GetMode() != Mode::SPAWN ? p_ghosts[g]->SetEdible(true) : p_ghosts[g]->SetEdible(false); // ghost only edible on power up if out of spwan area
            p_ghosts[g]->IsEdible() ? p_ghosts[g]->SetMode(Mode::RUN) : p_ghosts[g]->SetMode(p_ghosts[g]->GetMode());
            p_ghosts[g]->GetMode() == Mode::RUN ? p_ghosts[g]->SetReverseMove(true) : p_ghosts[g]->SetReverseMove(false);
        }
        if (p_level->level_mode != Mode::RUN)
            p_player->ClearEatenGohsts();
        p_level->level_mode = Mode::RUN;
        p_level->run_time_start = chrono::high_resolution_clock::now();
        p_level->eaten_pellets++;
        break;
    }
}

bool Game::NextLevelRestartGame()
{
    if (game_over)
    {

        string format = utility.Spacer("play again? 'y' = yes, 'n' = no", p_level->cols);
        cout << "\r";
        cout << format;
        cout << "play again? 'y' = yes, 'n' = no";
        cout << format;
        char input = _getch();
        if (input == Globals::kNO)
        {
            draw.ShowConsoleCursor(false);
            return false;
        }
        else
        {
            system("cls");
            current_scene = 1;
            return true;
        }
    }
    current_scene++;
    return true;
}

void Game::SetCollisionDelay()
{
    if (gobble_pause)
    {
        gobble_pause = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(Globals::gobble_delay)); // pause for ghost gobble
    }
    else if (player_beat_pause)
    {
        player_beat_pause = false;
        std::this_thread::sleep_for(std::chrono::milliseconds(Globals::player_beat_delay)); // pause for player gobble
    }
}

void Game::CheckLevelComplete()
{
    p_level->CheckLevelComplete();
}

void Game::SetRefreshDelay()
{
    if (!p_level->is_complete && !game_over)
    {
        std::this_thread::sleep_for(std::chrono::milliseconds(Globals::refresh_delay)); // pause to slow game
    }
}