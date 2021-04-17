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
            // move the player
            MovePlayer();

            // process any player / ghost collision
            player_died = CheckCollisions();

            if (!player_died)
            {
                // if move into power up, pellet, set the necessary status and update scores
                SetPlayerState();

                // set ghost chase modes as approprite
                SetGhostMode();

                // move the ghost
                MoveGhostsAndFruit();

                // process any player / ghost collision again
                player_died = CheckCollisions();
            }

            // delay render if there's a collision
            SetCollisionDelay();

            // Draw the level current state
            DrawLevel();

            // end condition for the game once user has eaten all pellets
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

    // reset fruit
    p_fruit->ResetFruit();

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
            CXBOXController* p_controller = new CXBOXController(1);
            SFX(Play::INTERMISSION);
            do
            {
                if (!p_controller->IsConnected())
                {
                    // try to connect again
                    delete p_controller;
                    p_controller = nullptr;
                    p_controller = new CXBOXController(1);
                }
                
                if (p_controller->IsConnected())
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
            
            // delete controller if it exists
            if (p_controller)
            {
                delete p_controller;
                p_controller = nullptr;
            }

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
    p_controller = new CXBOXController(1);
    int input, magnitude = 32767; // magintude is the value each stick can move max by
    float controllerX, controllerY; // will hold how far the stick is pushed on x/y axis

    do
    {
        if (p_controller && !p_controller->IsConnected())
        {
            delete p_controller;
            p_controller = nullptr;
            p_controller = new CXBOXController(1);
        }

        if (p_controller->IsConnected())
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

    // delete controller if it exists
    if (p_controller)
    {
        delete p_controller;
        p_controller = nullptr;
    }

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

                    // play the sound for bonus and animate eaten ghost
                    SFX(Play::LIFE);
                    p_player->EatGhostAnimate(g, true);

                }
                
                // play ghost eaten animation and respawn the ghost
                SFX(Play::EAT_GHOST);
                p_player->EatGhostAnimate(g, false);
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

        return true;

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

    return false;

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
    string format = Utility::Spacer("CCC-----00000000----0000", p_level->cols);
    string lives;
    
    for (int i = 0; i < 3; i++)
    {
        i < p_player->Lives() ? lives = lives + char(Globals::pacman_left_open) : lives = lives +  "-";
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
    //Draw::SetColor(14);
    //cout << "  " << p_fruit->ticks << ":" << p_fruit->spawn_count;
    //Draw::SetColor(7);

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
    bool continue_play = true;
    
    if (game_over)
    {
        string format = Utility::Spacer("play again? (y)es (n)o", p_level->cols);
        cout << "\r";
        cout << format;
        cout << "play again? (y)es (n)o";
        cout << format;
    }

    CXBOXController* p_controller = new CXBOXController(1);

    do
    {
        if (!p_controller->IsConnected())
        {
            // try to connect again
            delete p_controller;
            p_controller = nullptr;
            p_controller = new CXBOXController(1);
        }

        if (p_controller->IsConnected())
        {
            if (game_over && p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_X)
            {
                Draw::ShowConsoleCursor(false);
                continue_play = false;
                break;
            }
                    
            if (p_controller->GetState().Gamepad.wButtons & XINPUT_GAMEPAD_A)
            {
                system("cls");
                current_scene = 1;
                continue_play = true;
                break;
            }

        }
        else
        {
            char input = _getch();
            if (game_over && input == Globals::kNO)
            {
                Draw::ShowConsoleCursor(false);
                continue_play = false;
                break;
            }
            else
            {
                system("cls");
                continue_play = true;
                break;
            }
        }

    } while (true);
        
    // delete controller if it exists
    if (p_controller)
    {
        delete p_controller;
        p_controller = nullptr;
    }
    
    game_over ? current_scene = 1 : current_scene++;
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

Resolution Game::GetResolution()
{
    return res;
}