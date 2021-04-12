#include "Game.h"
#include <conio.h>
#include <iostream>
#include <Windows.h> // colors and play sounds
#include <mmsystem.h> // play .wav
#include <thread> // used to get input while running game
#include <iomanip> // formating output

using namespace std;

//constructors
Game::Game()
{
    // create level object and set the game ref
    p_level = new Level;
    p_level->SetGameRef(this);

    // create player object and pass it the game ref
    p_player = new Player;
    p_player->SetGameRef(this);

    // create the ghosts array and pass it the game ref
    p_ghosts = new Ghost* [Globals::total_ghosts] {};
    p_ghosts[0] = new Ghost(Ghosts::RED);
    p_ghosts[1] = new Ghost(Ghosts::YELLOW);
    p_ghosts[2] = new Ghost(Ghosts::BLUE);
    p_ghosts[3] = new Ghost(Ghosts::PINK);
    for (int g = 0; g < Globals::total_ghosts; g++)
    {
        p_ghosts[g]->SetGameRef(this);
    }

    // get the current resolution and store this
    res = Draw::GetResolution();

};

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

    system("cls");

}

// flow methods

void Game::SetupGame()
{
    // if restarting the game then reset player lives and game over state
    game_over ? p_player->SetLives(3) : p_player->SetLives();
    p_player->ClearEatenGohsts(); // clear the ghosts eaten in a row
    game_over = false;

    // reset ghosts to spawn state
    SpawnAllGhosts();

    // reset level, create scene and update player / level as needed
    p_level->SetupLevel(current_scene);

    // set console font to pacman font
    Draw::SetConsoleFont(true, res);
    Draw::SetConsoleSize(res, p_level->rows + 7, p_level->cols + 2);
}

void Game::DrawLevel()
{
    if (p_level->GameRefIsSet())
    {
        p_level->DrawLevel(); //draw level

        // pause before starting the level
        if (p_level->level_paused) {
            // before start of level get any key to start
            string format = Utility::Spacer("Press a key to start.", p_level->cols);
            cout << endl;
            cout << format;
            cout << "Press a key to start.";
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

void Game::MovePlayer()
{
    if (p_player->GameRefIsSet())
    {
        // move the player based on the content of the move to sqaure
        p_player->MovePlayer();

        // set any statuses that need setting
        SetPlayerState();

        // check collision with a ghost and set the ghosts previous row to the new player position
        for (int g = 0; g < Globals::total_ghosts; g++)
        {
            if (p_player->PayerGhostCollision(g)) {
                p_ghosts[g]->GetPreviousPosition().SetTo(p_player->GetCurrentPosition());
            }
        }
    }
    
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

void Game::PlayerMonsterCollision()
{
    bool player_died = false;

    for (int g = 0; g < Globals::total_ghosts; g++) // loop ghosts
    {
        if (p_player->GetCurrentPosition() == p_ghosts[g]->GetCurrentPosition())
        {
            DrawLevel(); // print the move immediately
            if (p_ghosts[g]->IsEdible()) // ghost dies
            {
                // if the ghost was on a pellet/powerup sqaure need to count it up
                if (p_ghosts[g]->GetContentCurrent() == (char)Globals::pellet || p_ghosts[g]->GetContentCurrent() == (char)Globals::powerup)
                    p_level->eaten_pellets++;
                p_level->eaten_ghosts++; // increment ghosts eaten
                gobble_pause = true; // set small pause after eating ghost
                p_ghosts[g]->SpawnGhost(p_ghosts[g]->Name(), false);
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

void Game::MoveGhosts()
{
    for (int g = 0; g < Globals::total_ghosts; g++)
    {
        if(p_ghosts[g]->GameRefIsSet())
            p_ghosts[g]->MakeGhostMove();
    }
}

void Game::PrintStatusBar()
{
    //Draw draw;
    //Utility utility;
    // get number of lives
    string format = Utility::Spacer("C C C    00000000", p_level->cols);
    string lives;
    for (int i = 0; i < 6; i++)
    {
        i < p_player->Lives() ? lives = lives + char(Globals::pacman_left_open) + " " : lives = lives +  " ";
    }
    p_player->SetScore(((p_level->eaten_pellets + 1) * p_level->points_pellet) + (p_level->eaten_ghosts * p_level->points_ghost) + (p_level->eaten_ghosts >= 4 ? p_level->all_ghost_bonus : 0));
    cout << endl;
    cout << format;
    Draw::SetColor(14);
    cout << (!p_player->HasNoLives() ? lives : "-");
    Draw::SetColor(7);
    cout << " ";
    Draw::SetColor(14);
    cout << setfill('0') << setw(8) << p_player->GetScore();
    Draw::SetColor(7);
    cout << format;
    cout << "\r";

    // message game over or level complete
    if (game_over)
    {
        format = Utility::Spacer("Game Over! Press a key.", p_level->cols);
        cout << format;
        cout << "Game Over! Press a key.";
        cout << format;
    }
    else if (p_level->is_complete)
    {
        format = Utility::Spacer("Level complete! Press a key.", p_level->cols);
        cout << format;
        cout << "Level complete! Press a key.";
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
            if (p_ghosts[g]->GetCurrentPosition() == p_level->ghost_spawn)
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

bool Game::NextLevelRestartGame()
{
    if (game_over)
    {

        string format = Utility::Spacer("play again? (y)es (n)o", p_level->cols);
        cout << "\r";
        cout << format;
        cout << "play again? (y)es (n)o";
        cout << format;
        char input = _getch();
        if (input == Globals::kNO)
        {
            Draw::ShowConsoleCursor(false);
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


// other methods
void Game::SpawnAllGhosts()
{
    if (p_ghosts)
    {
        for (int i = 0; i < Globals::total_ghosts; i++)
        {
            p_ghosts[i]->SpawnGhost(p_ghosts[i]->Name(), true);
        }
    }

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

bool Game::IsGameOver()
{
    return game_over;
}

Resolution Game::GetResolution()
{
    return res;
}