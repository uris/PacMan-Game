#pragma once
#include "EnumsAndStatics.h"
#include "Player.h"
#include "Ghost.h"
#include "Level.h"
#include "Utility.h"
#include "Draw.h"
#include "Fruit.h"
#include "CXBOXController.h"
#include <chrono>

using namespace std::chrono;

class Game
{
    // Game States
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;

    // Game level
    int current_scene = 1;
    Resolution res = Resolution::NORMAL;

    //SFX
    Play sfx = Play::NONE;
    high_resolution_clock::time_point sfx_start = std::chrono::high_resolution_clock::now();

    // xbox controller -- will use this to get player direction
    CXBOXController* p_controller = nullptr;

    public:
        // level - public for class forwarding
        Level* p_level = nullptr;

        // player - public for class forwarding
        Player* p_player = nullptr;

        // ghosts -- public for class forwarding
        Ghost** p_ghosts = nullptr;

        // fruit -- public for class forwarding
        Fruit* p_fruit = nullptr;
        
        // game constructors
        Game();

        // game destructors
        ~Game();

        // game flow
        void RunGame();
        void SetupGame();
        void MovePlayer();
        void MoveGhosts();
        bool NextLevelRestartGame();
        void PrintStatusBar();
        void DrawLevel();
        bool IsGameOver();
        
        // game orchestration methods
        void CheckCollisions();
        void CheckLevelComplete();
        void NextScene();
        void GetKeyboardInput();
        void SpawnAllGhosts();
        int SFX(Play playSFX);
        void SetPlayerState();
        void SetGhostMode();
        void SetCollisionDelay();
        void SetRefreshDelay();
        Resolution GetResolution();
        
};
