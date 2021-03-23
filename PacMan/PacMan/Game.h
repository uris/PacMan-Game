#pragma once
#include "Enums.h"
#include "Player.h"
#include "Ghost.h"
#include "Level.h"
#include "Utility.h"
#include "Draw.h"
#include <chrono>

using namespace std::chrono;

class Game
{
    // helper
    Utility utility;
    Draw draw;
    
    // level pointer
    Level* p_level = nullptr;
    
    // player pointer
    Player* p_player = nullptr;

    // pointer to array of ghosts
    Ghost* p_ghosts = nullptr;
    int total_ghosts = 4;
    
    // Game States
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;

    // Game level
    int total_scenes = 2;
    int current_scene = 1;

    //SFX
    Play sfx = Play::NONE;
    high_resolution_clock::time_point sfx_start = std::chrono::high_resolution_clock::now();

    public:

        // game player movement and input keys
        static constexpr char kLEFT = 97; // 'a';
        static constexpr char kUP = 119; // 'w';
        static constexpr char kDOWN = 115; // 's';
        static constexpr char kRIGHT = 100; // 'd';
        static constexpr char kARROW_UP = 72;
        static constexpr char kARROW_DOWN = 80;
        static constexpr char kARROW_LEFT = 75;
        static constexpr char kARROW_RIGHT = 77;
        static constexpr char kESCAPE = 27;
        static constexpr char kYES = 121; // 'y'
        static constexpr char kNO = 110; // 'n'

        // game delay defaults
        static constexpr int gobble_delay = 750; // wait in milliseconds
        static constexpr int player_beat_delay = 1000; // wait in milliseconds
        static constexpr int refresh_delay = 300; //milliseconds

        // game constructors
        Game();

        // game destructors
        ~Game();

        // game flow
        void RunGame();

        // game functions
        void GameCredits();
        void SetupGame();
        void MovePlayer();
        int MoveGhosts();
        int GetBestMove(int g, Coord move, Direction curr_direction, int depth, bool isGhost);
        void DoGhostMove(int g, Direction direction);
        char GetSquareContentNow(int g, Direction direction);
        Direction RandomGhostMove(int g);
        void SetPlayerState();
        void PlayerMonsterCollision();
        void SetGhostMode();
        void RefreshDelay();
        void PlayerMonsterCollisionDelay();
        void CheckLevelComplete();
        bool NextLevelRestartGame();
        void StatusBar();
        void NextScene();
        void GetKeyboardInput();
        void Add(Player& player);
        void Add(Ghost& red, Ghost& yellow, Ghost& blue, Ghost& pink);
        void Add(Level& level);
        void SpawnThisGhost(Ghosts name, bool player_died);
        void SpawnAllGhosts();
        int SFX(Play playSFX);
        void DrawLevel();
        
};
