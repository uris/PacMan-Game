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
    
    // level
    Level* p_level = nullptr;
    
    // player
    Player* p_player = nullptr;

    // ghosts
    Ghost** p_ghosts = nullptr;
    
    // Game States
    bool game_over = false;
    bool gobble_pause = false;
    bool player_beat_pause = false;

    // Game level
    int current_scene = 1;
    char map_contents[4]{};

    //SFX
    Play sfx = Play::NONE;
    high_resolution_clock::time_point sfx_start = std::chrono::high_resolution_clock::now();

    public:
        // game constructors
        Game();

        // game destructors
        ~Game();

        // game flow
        void RunGame();
        void GameCredits();
        void SetupGame();
        void MovePlayer();
        int MoveGhosts();
        int GetBestMove(int g, Coord new_position, Direction current_direction, int depth);
        char GetMapContent(Coord map_coord, Direction direction);
        void SetMapContents(Coord map_coord);
        bool PlayerGhostCollision(const int g);
        bool NextLevelRestartGame();
        void PrintStatusBar();
        void DrawLevel();
        
        // game orchestration methods
        char GhostContentNow(char ghost_char);
        Direction RandomGhostMove(int g);
        void PlayerMonsterCollision();
        void CheckLevelComplete();
        void NextScene();
        void GetKeyboardInput();
        void SpawnThisGhost(Ghosts name, bool player_died);
        void SpawnAllGhosts();
        int SFX(Play playSFX);
        void Add(Player* player);
        void Add(Ghost* red, Ghost* yellow, Ghost* blue, Ghost* pink);
        void Add(Level* level);
        void SetPlayerState();
        void SetGhostMode();
        void SetCollisionDelay();
        void SetRefreshDelay();  
        
};
