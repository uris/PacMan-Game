#pragma once
#include "EnumsAndStatics.h"
#include "Player.h"
#include "Ghost.h"
#include "Level.h"
#include "Utility.h"
#include "Draw.h"
#include <chrono>

using namespace std::chrono;

class Game
{
    // helper objects for utility / qol
    Utility utility;
    Draw draw;
    
    // Game States
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;

    // Game level
    int current_scene = 1;

    //SFX
    Play sfx = Play::NONE;
    high_resolution_clock::time_point sfx_start = std::chrono::high_resolution_clock::now();

    public:
        // level - public for class forwarding
        Level* p_level = nullptr;

        // player - public for class forwarding
        Player* p_player = nullptr;

        // ghosts -- public for class forwarding
        Ghost** p_ghosts = nullptr;
        
        // game constructors
        Game();

        // game destructors
        ~Game();

        // game flow
        void RunGame();
        void GameCredits();
        void SetupGame();
        void MovePlayer();
        void MoveGhosts();
        bool NextLevelRestartGame();
        void PrintStatusBar();
        void DrawLevel();
        bool IsGameOver();
        
        // game orchestration methods
        void PlayerMonsterCollision();
        void CheckLevelComplete();
        void NextScene();
        void GetKeyboardInput();
        void SpawnAllGhosts();
        int SFX(Play playSFX);
        void SetPlayerState();
        void SetGhostMode();
        void SetCollisionDelay();
        void SetRefreshDelay();  
        
};
