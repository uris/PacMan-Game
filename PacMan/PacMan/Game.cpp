#include "Game.h"
#include <conio.h>
#include <iostream>
#include <fstream> // file manipulation
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

    // create player object and pass it the game ref
    p_fruit = new Fruit;
    p_fruit->SetGameRef(this);

    // create the fx objects and set up sounds in memory
    /*p_sounds = new SoundFX * [12] {};
    p_sounds[0] = new SoundFX("sfx_bonus.wav");
    p_sounds[1] = new SoundFX("sfx_death.wav");
    p_sounds[2] = new SoundFX("sfx_eatfruit.wav");
    p_sounds[3] = new SoundFX("sfx_eatghost.wav");
    p_sounds[4] = new SoundFX("sfx_intermission.wav");
    p_sounds[5] = new SoundFX("sfx_intro.wav");
    p_sounds[6] = new SoundFX("sfx_munch.wav");
    p_sounds[7] = new SoundFX("sfx_munch_lg.wav");
    p_sounds[8] = new SoundFX("sfx_powerup.wav");
    p_sounds[9] = new SoundFX("sfx_retreat.wav");
    p_sounds[10] = new SoundFX("sfx_siren.wav");
    p_sounds[11] = new SoundFX("sfx_xlife.wav");*/

    // get the current resolution and store this
    res = Draw::GetResolution(24);

};

//destructors
Game::~Game()
{
    // fruit pointer to null
    if (p_fruit) {
        delete p_fruit;
        p_fruit = nullptr;
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

    // delete ghosts array and set pointer to null
    if (p_ghosts) {
        for (int i = 0; i < Globals::total_ghosts; i++)
        {
            delete p_ghosts[i];
        }
        delete[] p_ghosts;
        p_ghosts = nullptr;
    }

    if (p_controller) {
        delete p_controller;
        p_controller = nullptr;
    }

    // delete the sounds array and set to null
    /*if (p_sounds) {
        for (int i = 0; i < 12; i++)
        {
            if (p_sounds[i])
            {
                delete p_sounds[i];
            }
  
        }
        delete[] p_sounds;
        p_sounds = nullptr;
    }*/
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
        thread inputThread(&Game::GetUserInput, this); // new the ref() wrapper to pass by ref into thread

        do
        {
            // check for level complete before player move
            level = CheckLevelComplete();

            // do the player movement and checks (ignore if player dead or level complete)
            if (player && level)
            {
                // move the player
                MovePlayer();

                // process any player / ghost / fruit collisions (player false if dies)
                player = CheckCollisions();

                // check if level was complete with the move
                level = CheckLevelComplete();
            }

            // do the NPC movement and checks (ignore if player dead or level is complete)
            if (player && level)
            {
                // if move into power up, pellet, set the necessary status and update scores
                SetPlayerState();

                // set ghost chase modes (roam, chase, run) as approprite
                SetGhostMode();

                // move the ghost and the fruit if it is active
                MoveGhostsAndFruit();

                // process any player / ghost collision again
                player = CheckCollisions();
            }

            // delay render if there's a collision
            SetCollisionDelay();

            // Draw the level current state
            DrawLevel();

            // display stats and number lives
            PrintStatusBar();

            // introduce a wait for fast PC
            SetRefreshDelay();

        } while (level && !game_over);

        // kill any sounds playing one level complet or game over
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
    
    

    if (game_over) // if restarting the game then reset player
    {
        // 3 lives, score at 0, etc.
        p_player->ResetPlayer();
    }

    // reset all other object variables for both new game and new level
    ResetGame();
    p_fruit->ResetFruit();
    p_level->ResetLevel();
    SpawnAllGhosts();

    // set up the level based on the current scene
    int check = p_level->SetupLevel(current_scene);

    // if check os -1 then we reached the end of the game!
    if (check != 1)
    {
        
        // show the high scores and ask to play again
        ShowHighScores();
        bool keep_playing = PlayOrExit();

        // if not keep playing then exit.
        if (!keep_playing)
            exit(0);

        // otherwise reset the player to continue game
        p_player->ResetPlayer();

    }

    // set console font to pacman font
    Draw::SetConsoleFont(true, res);
    Draw::SetConsoleSize(res, p_level->rows + 7, p_level->cols);

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
            
            p_controller = new CXBOXController(1);
            bool is_connected = p_controller->IsConnected();

            SFX(Play::INTERMISSION);
            do
            {
                if (is_connected)
                {
                    if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A || abs(p_controller->GetState().Gamepad.sThumbLX) > 2500 || abs(p_controller->GetState().Gamepad.sThumbLY) > 2500)
                        break;
                }
                else
                {
                    char input = _getch();
                    break;
                }
                

            } while (true);

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

void Game::GetUserInput()
{
    
    int input, magnitude = 32767; // magintude is the value each stick can move max by
    float controllerX, controllerY; // will hold how far the stick is pushed on x/y axis

    p_controller = new CXBOXController(1);
    bool is_connected = p_controller->IsConnected();

    do
    {

        if (is_connected)
        {
            
            // get the x / y of the left control stick
            controllerX = p_controller->GetState().Gamepad.sThumbLX;
            controllerY = p_controller->GetState().Gamepad.sThumbLY;

            if (abs(controllerY) > abs(controllerX)) // do up or down
            {
                if (controllerY > (magnitude / 2.0f))
                    p_player->SetDirection(Direction::UP);
                else if (controllerY < -(magnitude / 2.0f))
                    p_player->SetDirection(Direction::DOWN);
            }
            else // do left or right
            {
                if (controllerX > (magnitude / 2.0f))
                    p_player->SetDirection(Direction::RIGHT);
                else if (controllerX < -(magnitude / 2.0f))
                    p_player->SetDirection(Direction::LEFT);
            }

            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_UP)
                p_player->SetDirection(Direction::UP);

            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_RIGHT)
                p_player->SetDirection(Direction::RIGHT);

            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_DOWN)
                p_player->SetDirection(Direction::DOWN);

            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_DPAD_LEFT)
                p_player->SetDirection(Direction::LEFT);

        }
        
        else
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
        }
        
    } while (!p_level->is_complete && !game_over);

}

void Game::MovePlayer()
{
    if (p_player->GameRefIsSet())
    {
        // update the player position based on direction that is set
        p_player->MovePlayer();

        // place and empty sapace in the position the player just moved out of
        p_player->UpdateMapAfterMove();

    }
}

bool Game::CheckLevelComplete()
{
    return !p_level->CheckLevelComplete();
}

bool Game::CheckCollisions()
{
    bool player_died = false;

    // process collisions with ghosts
    for (int g = 0; g < Globals::total_ghosts; g++) // loop ghosts
    {
        if (p_player->PlayerGhostCollision(g))
        {
            
            DrawLevel(); // print the move immediately so we can pause / reset as needed
            
            if (p_ghosts[g]->IsEdible()) // ghost dies and respawns, player gets points
            {
                gobble_pause = true; // set small pause after eating ghost to do animation
                
                p_player->EatGhost(g); // add eaten ghost to array of ghosts eaten in same powerup
                p_player->IncrementScore(Object::GHOST); // add the eaten ghost to the score

                if (p_player->AllGhostsEaten()) { // if all ghosts eaten in one powerup
                    // add the bonus to the player score
                    p_player->IncrementScore(Object::ALL_GHOSTS);
                    
                    // reset all eaten ghosts marker, set the mode back to chaase and restart th chase timer
                    p_player->ClearEatenGohsts();
                    p_level->level_mode = Mode::CHASE;
                    p_level->chase_time_start = chrono::high_resolution_clock::now();

                    // play the sound for bonus
                    SFX(Play::LIFE);
                }
                else
                {
                    // play ghost eaten animation
                    SFX(Play::EAT_GHOST);
                }
                // animate player and respawn ghost
                p_player->EatGhostAnimate(g, true);
                p_ghosts[g]->SpawnGhost(p_ghosts[g]->Name(), false);
            }

            else // player dies
            
            {

                // player dead first times plays the player died animation
                if (!player_died && !game_over)
                {
                    SFX(Play::DEATH);
                    p_player->DeathAnimate(g);
                }
                // set player died to true
                player_died = true;

            }
        }
    }

    if (player_died) {

        // kill fruits is there are any
        p_fruit->KillFruit();

        // reset the roam count by adding one back to the count if ghosts were roaming
        p_level->level_mode == Mode::ROAM ? p_level->roam_count++ : p_level->roam_count;

        // set the level mode to chase
        p_level->level_mode = Mode::CHASE;
        p_level->chase_time_start = chrono::high_resolution_clock::now();

        // rediuce the player life
        p_player->TakeLives(1);
        
        // end game if no more lives, else respawn all
        if (!p_player->HasNoLives())
        {
            player_beat_pause = true; // give player time to get ready
            
            // respawn player
            p_player->ReSpawn();

            // respawn ghosts
            SpawnAllGhosts();
        }
        else
        {
            game_over = true;
        }

        return false; // player no longer active in game cycle

    }

    // process collision with fruit
    if (p_player->PlayerFruitCollision() && p_fruit->FruitActive())
    {
        DrawLevel(); // print the move immediately
        
        // update stats
        p_player->IncrementScore(Object::FRUIT);
        p_fruit->SetFruitEaten();
        p_fruit->KillFruit(true);
        
        
        SFX(Play::EAT_FRUIT); //play sfx
    }

    return true; // player still active in game cycle

}

void Game::SetPlayerState()
{
    char levelObj = p_level->p_map[p_player->GetCurrentRow()][p_player->GetCurrentCol()];
    switch (levelObj)
    {
    case (char)Globals::pellet: // eat pellet
        p_player->IncrementScore(Object::PELLET);
        break;
    case (char)Globals::powerup: // eat power up
        p_player->IncrementScore(Object::PELLET);
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
        break;
    }
}

void Game::MoveGhostsAndFruit()
{
    for (int g = 0; g < Globals::total_ghosts; g++)
    {
        if(p_ghosts[g]->GameRefIsSet())
            p_ghosts[g]->MakeGhostMove();
    }
    if (p_fruit->GameRefIsSet())
    {
        p_fruit->MakeFruitMove();
    }
}

void Game::PrintStatusBar()
{
    //Draw draw;
    //Utility utility;
    // get number of lives
    string format = Utility::Spacer("CCC----00000000----0000", p_level->cols);
    string lives;
    
    for (int i = 1; i < 3; i++)
    {
        i < p_player->Lives() ? lives = lives + char(Globals::pacman_left_open) : lives = lives +  " ";
    }
    
    cout << endl;
    cout << format;
    
    Draw::SetColor(14);
    cout << (!p_player->HasNoLives() ? lives : "-");
    Draw::SetColor(7);
    
    cout << "     ";
    
    Draw::SetColor(14);
    cout << setfill('0') << setw(8) << p_player->GetScore();
    Draw::SetColor(7);

    cout << "    ";
    
    p_player->CoutEatenFruits();

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

void Game::SetRefreshDelay()
{
    if (!p_level->is_complete && !game_over)
    {
        if (p_controller && p_controller->IsConnected())
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Globals::refresh_delay_controller)); // pause to slow game
        }
        else
        {
            std::this_thread::sleep_for(std::chrono::milliseconds(Globals::refresh_delay)); // pause to slow game
        }
        
    }
}

bool Game::NextLevelRestartGame()
{
    bool continue_play = true;
    
    if (game_over)
    {
        ShowHighScores();
        continue_play = PlayOrExit();
    }
    
    return continue_play;
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

    // reset player back to true
    player = true;
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
        if (sfx == Play::DEATH)
            return 0;
        t_sfx = TEXT("sfx_death.wav");
        params = SND_FILENAME | SND_ASYNC;
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
        params = SND_FILENAME | SND_ASYNC;
        break;
    case Play::EAT_GHOST:
        t_sfx = TEXT("sfx_eatghost.wav");
        params = SND_FILENAME | SND_ASYNC;
        break;
    case Play::EAT_FRUIT:
        if (sfx == Play::EAT_FRUIT)
            return 0;
        t_sfx = TEXT("sfx_eatfruit.wav");
        params = SND_FILENAME | SND_SYNC;
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
        if (playSFX == Play::DEATH || playSFX == Play::EAT_GHOST || playSFX == Play::LIFE || playSFX == Play::EAT_FRUIT)
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

bool Game::IsGameOver()
{
    return game_over;
}

void Game::ResetGame()
{
    game_over ? current_scene = 1 : current_scene++;
    player = true;
    level = true;
    game_over = false;
    gobble_pause = false;
    player_beat_pause = false;
}

Resolution Game::GetResolution()
{
    return res;
}

void Game::ReadSaveHighScores(const string scores)
{

    if (scores == "")
    {
        ifstream score_board("PacManS.scores");
        string file_line, marker;
        int index = 0;

        if (score_board) {

            // file exists and is open 
            while (getline(score_board, file_line))
            {
                // get the user
                if (file_line.find(':', 0) != std::string::npos)
                {
                    high_scores[index][0] = file_line.substr(0, file_line.find(':', 0));
                    high_scores[index][1] = file_line.substr(file_line.find(':', 0) + 1, file_line.length() - file_line.find(':', 0));
                    index++;
                }
            }
            score_board.close();
        }
        else
        {
            //unable to read file - write sine error code. For now not polished.
            system("cls");
            cout << "Can't seem to load the PacMan scenes. Check your code man!" << endl;
            system("pause");
            exit(0);
        }
    }
    else
    {
        // update the high scores file
        ofstream score_board("PacManS.scores");
        score_board << scores;
        score_board.close();
    }
   

}

void Game::ShowHighScores()
{
    // Draw the high scores
    string str_high_scores = PrintHighScores();

    // if there's a new high score get name and save to file
    if (str_high_scores.find("#new#", 0) != std::string::npos)
    {
        
        // ask for and get the user name
        string name = "";
        cout << endl << endl << " Nice! new high score." << endl << " Enter your name." << endl << endl << " ";
        Draw::ShowConsoleCursor(true);
        getline(cin, name);

        // if blank set the name to player
        if (name == "")
            name = "Player";

        // trim the name to 9 spaces only
        if (name.length() > 9)
            name = name.substr(0, 9);

        // replace the new slot with trimmed user name
        Utility::ReplaceString(str_high_scores, "#new#", name);
        
        // update scores file
        ReadSaveHighScores(str_high_scores);

        // output the high scores without updates
        PrintHighScores(true);
    }
}

string Game::PrintHighScores(const bool no_update)
{
    // clear screen
    system("cls");

    // remove cursor
    Draw::ShowConsoleCursor(false);

    // Draw Title
    string format = "";
    Draw::SetColor(Globals::cWHITE);
    cout << endl;

    Draw::SetColor(Globals::cPELLETS);
    format = Utility::Spacer("[{}", 30);
    cout << format;
    cout << "[{} ";
    cout << format;

    cout << endl;

    Draw::SetColor(7);
    format = Utility::Spacer("high scores", 30);
    cout << format;
    cout << "High Scores";
    cout << format;

    cout << endl << endl;
    
    // Reac scores from file
    ReadSaveHighScores();

    int count = 0, index = 0, score = 0, color = 7;
    string name, spaces, str_high_scores = "";
    bool new_high_score = false;
    format = Utility::Spacer("000 AAAAAAAAA  #########", 30);

    while (count < 10)
    {

        spaces = "";
        score = stoi(high_scores[index][0]);

        if (!no_update && !new_high_score && p_player->GetScore() > score)
        {
            name = " ";
            score = 50023;
            score = p_player->GetScore();
            new_high_score = true;
            color = 368; //(black on white bg);
        }
        else
        {
            name = high_scores[index][1];
            index++;
            color = Globals::cPELLETS; //yellow

        }

        // create the name string up to 10 chars
        size_t len = name.length();
        for (int i = 0; i < 10 - len; i++)
        {
            spaces += " ";
        }

        // print out the score & create the high scores string
        cout << format;
        Draw::SetColor(color);
        cout << setfill('0') << setw(2) << count + 1 << ". ";
        cout << name << spaces << " ";
        cout << setfill('0') << setw(9) << score;
        Draw::SetColor(7);
        cout << endl << endl;
        str_high_scores += to_string(score) + ":" + (name == " " ? "#new#" : name) + "\n";

        // increase the counter
        count++;
    }

    return str_high_scores;
}

bool Game::PlayOrExit()
{
    bool continue_play = true;

    string format = Utility::Spacer("play again? (y)es (n)o", p_level->cols);
    cout << endl;
    cout << format;
    cout << "play again? (y)es (n)o";
    cout << format;

    p_controller = new CXBOXController(1);
    bool is_connected = p_controller->IsConnected();


    do
    {
        // if a controller is connected use that as input
        if (is_connected)
        {
            // Pressing "X" exits the game
            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
            {
                Draw::ShowConsoleCursor(false);
                continue_play = false;
                break;
            }

            // Pressing "A" restars the game
            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
            {
                system("cls");
                continue_play = true;
                break;
            }
        }
        else // if no controller, get the input from the keyboard
        {
            char input = _getch();
            if (input == Globals::kNO)
            {
                // Pressing "n" exits the game                
                Draw::ShowConsoleCursor(false);
                continue_play = false;
                break;
            }
            else
            {
                // Any key but "n" restarts the game
                system("cls");
                continue_play = true;
                break;
            }
        }

    } while (true);

    return continue_play;
}

