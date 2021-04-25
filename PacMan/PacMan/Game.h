#pragma once
#include "EnumsAndStatics.h"
#include "Character.h"
#include "Player.h"
#include "Ghost.h"
#include "Level.h"
#include "Utility.h"
#include "Draw.h"
#include "Fruit.h"
#include "SoundFX.h"
#include "CXBOXController.h"
#include <chrono>

using namespace std::chrono;

class Game
{
    // Game States
    bool player = true;
    bool level = true;
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;

    // Game level
    int current_scene = 0;
    Resolution res = Resolution::NORMAL;

    //SFX
    Play sfx = Play::NONE;
    high_resolution_clock::time_point sfx_start = std::chrono::high_resolution_clock::now();

    // xbox controller -- will use this to get player direction
    CXBOXController* p_controller = nullptr;

    // high scores
    string high_scores[10][2]{};

    public:
        // level - public for class forwarding
        Level* p_level = nullptr;

        // player - public for class forwarding
        Player* p_player = nullptr;

        // ghosts -- public for class forwarding
        Ghost** p_ghosts = nullptr;

        // fruit -- public for class forwarding
        Fruit* p_fruit = nullptr;

        //// pointer to array of sounds stored in memory (faster playback)
        //SoundFX** p_sounds = nullptr;
        
        // game constructors
        Game();

        // game destructors
        ~Game();

        // game flow
        void RunGame();
        void SetupGame();
        void MovePlayer();
        void MoveGhostsAndFruit();
        bool NextLevelRestartGame();
        void PrintStatusBar();
        void DrawLevel();
        bool IsGameOver();
        
        // game orchestration methods
        bool CheckCollisions();
        bool CheckLevelComplete();
        void NextScene();
        void GetUserInput();
        void SpawnAllGhosts();
        int SFX(Play playSFX);
        void SetPlayerState();
        void SetGhostMode();
        void SetCollisionDelay();
        void SetRefreshDelay();
        Resolution GetResolution();
        void ResetGame();
        void ReadSaveHighScores(const string scores = "");
        void ShowHighScores();
        string PrintHighScores(const bool no_update = false);
        bool PlayOrExit();
        
};
