#include "Game.h"
#include <conio.h>

using namespace std;

Game::Game() {};

Game::Game(Player& player, Ghost& red, Ghost& yellow, Ghost& blue, Ghost& pink)
{
    p_player = &player;
    p_ghosts = new Ghost[4]{ red, yellow, blue, pink };
};

//Game::Game(Player& player, Level& level)
//{
//    p_player = &player;
//    p_level = &level;
//};

//destructors
Game::~Game()
{
    // player pointer to null
    p_player = nullptr;

    // delete ghosts array and set pointer to null
    delete[] p_ghosts;
    p_ghosts = nullptr;
    
};

bool Game::IsGameOn()
{
	return(game_on);
}

void Game::SetGameOn(bool game_on)
{
	this->game_on = game_on;
}

bool Game::IsGameOver()
{
	return(game_over);
}

void Game::SetGameOver(bool game_over)
{
	this->game_over = game_over;
}

bool Game::PauseForGobble()
{
	return(gobble_pause);
}

void Game::SetPauseForGobble(bool pause)
{
	gobble_pause = pause;
}

bool Game::PauseForPalyerDeath()
{
	return(player_beat_pause);
}

void Game::SetPauseForPlayerDeath(bool pause)
{
	player_beat_pause = pause;
}

int Game::GetCurrentScene()
{
	return(current_scene);
}

void Game::SetCurrentScene(int scene)
{
	current_scene = scene;
}

void Game::NextScene()
{
	current_scene++;
}

void Game::SetTotalScenes(int scenes)
{
	total_scenes = scenes;
}

void Game::SetPlaySFX(Play sfx)
{
	this->sfx = sfx;
}

Play Game::GetPlaySFX()
{
	return(sfx);
}

void Game::StartSFX()
{
	sfx_start = std::chrono::high_resolution_clock::now();
}

void Game::Add(Player& player, Ghost& red, Ghost& yellow, Ghost& blue, Ghost& pink)
{
    p_player = &player;
    p_ghosts = new Ghost[4]{red, yellow, blue, pink};

}

void Game::AddPlayer(Player& player)
{
    p_player = &player;
}

Player* Game::GetPlayer()
{
    return(p_player);
}

Ghost* Game::GetGhosts()
{
    return p_ghosts;
}

//void Game::AddLevel(Level& level)
//{
//    p_level = &level;
//}
//
//Level* Game::GetLevel()
//{
//    return(p_level);
//}

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
        case Game::kARROW_LEFT:
        case Game::kLEFT:
            p_player->SetDirection(Direction::LEFT);
            break;
        case Game::kARROW_RIGHT:
        case Game::kRIGHT:
            p_player->SetDirection(Direction::RIGHT);
            break;
        case Game::kARROW_UP:
        case Game::kUP:
            p_player->SetDirection(Direction::UP);
            break;
        case Game::kARROW_DOWN:
        case Game::kDOWN:
            p_player->SetDirection(Direction::DOWN);
            break;
        default:
            break;
        }
    } while (/*!p_level->is_complete &&*/ !game_over);
}