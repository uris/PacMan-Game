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
            delete[] p_ghosts[i];
        }
        delete[] p_ghosts;
        p_ghosts = nullptr;
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
            CheckCollisions();

            // set ghost chase modes as approprite
            SetGhostMode();

            ////// move the ghost
            MoveGhosts();

            // process any player / ghost collision again
            CheckCollisions();

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
    p_controller = new CXBOXController(1);
    
    int input, magnitude = 32767; // magintude is the value each stick can move max by
    float controllerX, controllerY;


    do
    {
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

    /*
    These are the buttons and controls of the xbox controller
    https://docs.microsoft.com/en-us/windows/win32/api/xinput/ns-xinput-xinput_gamepad?redirectedfrom=MSDN
    
    XINPUT_GAMEPAD_DPAD_UP          0x00000001
    XINPUT_GAMEPAD_DPAD_DOWN        0x00000002
    XINPUT_GAMEPAD_DPAD_LEFT        0x00000004
    XINPUT_GAMEPAD_DPAD_RIGHT       0x00000008
    XINPUT_GAMEPAD_START            0x00000010
    XINPUT_GAMEPAD_BACK             0x00000020
    XINPUT_GAMEPAD_LEFT_THUMB       0x00000040
    XINPUT_GAMEPAD_RIGHT_THUMB      0x00000080
    XINPUT_GAMEPAD_LEFT_SHOULDER    0x0100
    XINPUT_GAMEPAD_RIGHT_SHOULDER   0x0200
    XINPUT_GAMEPAD_A                0x1000
    XINPUT_GAMEPAD_B                0x2000
    XINPUT_GAMEPAD_X                0x4000
    XINPUT_GAMEPAD_Y                0x8000

    sThumbLX
    Left thumbstick x-axis value. Each of the thumbstick axis members is a signed value between -32768 and 32767 describing the position of the thumbstick. A value of 0 is centered. Negative values signify down or to the left. Positive values signify up or to the right. The constants XINPUT_GAMEPAD_LEFT_THUMB_DEADZONE or XINPUT_GAMEPAD_RIGHT_THUMB_DEADZONE can be used as a positive and negative value to filter a thumbstick input.

    sThumbLY
    Left thumbstick y-axis value. The value is between -32768 and 32767.

    sThumbRX
    Right thumbstick x-axis value. The value is between -32768 and 32767.

    sThumbRY
    Right thumbstick y-axis value. The value is between -32768 and 32767.

    */
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
            if (p_player->PlayerGhostCollision(g)) {
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
        p_player->IncrementScore(Object::PELLET);
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
        p_player->IncrementScore(Object::PELLET);
        break;
    }
}

void Game::CheckCollisions()
{
    bool player_died = false;


    // process ghost colission
    for (int g = 0; g < Globals::total_ghosts; g++) // loop ghosts
    {
        if (p_player->PlayerGhostCollision(g))
        {
            DrawLevel(); // print the move immediately
            if (p_ghosts[g]->IsEdible()) // ghost dies
            {
                // if the ghost was on a pellet/powerup sqaure need to count it up
                if (p_ghosts[g]->GetContentCurrent() == (char)Globals::pellet || p_ghosts[g]->GetContentCurrent() == (char)Globals::powerup)
                {
                    //p_level->eaten_pellets++; // TODO - remove
                    p_player->IncrementScore(Object::PELLET);
                }
                    
                
                p_level->eaten_ghosts++; // increment ghosts eaten
                p_player->IncrementScore(Object::ALL_GHOSTS);
                
                gobble_pause = true; // set small pause after eating ghost
                p_ghosts[g]->SpawnGhost(p_ghosts[g]->Name(), false);
                p_player->EatGhost(g);
                if (p_player->AllGhostsEaten()) {
                    
                    //p_level->all_eaten_ghosts += 1; // TODO - remove
                    p_player->IncrementScore(Object::ALL_GHOSTS);
                    
                    SFX(Play::LIFE);
                    p_player->EatGhostAnimate(g, true);

                    p_player->ClearEatenGohsts();
                    p_level->level_mode = Mode::CHASE;
                    p_level->chase_time_start = chrono::high_resolution_clock::now();
                }
                else
                {
                    SFX(Play::EAT_GHOST);
                    p_player->EatGhostAnimate(g, false);
                }

            }
            else
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

    if (p_player->PlayerFruitCollision() && p_fruit->FruitActive())
    {
        DrawLevel(); // print the move immediately
        p_fruit->SetFruitEaten();
        SFX(Play::EAT_FRUIT);
        p_fruit->KillFruit(true);
        p_player->AddEatenFruit(p_fruit->FruitType());
        if (p_fruit->GetContentCurrent() == (char)Globals::pellet || p_fruit->GetContentCurrent() == (char)Globals::powerup)
        {
            p_level->eaten_pellets++;
            p_player->IncrementScore(Object::PELLET);
        }
        // TODO: if powerup need to set the game mode
    }

    if (player_died) {

        // kill fruits is there are any
        p_fruit->KillFruit();
        
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
            
            
        }
        else
        {
            game_over = true;
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