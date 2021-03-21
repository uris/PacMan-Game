#pragma once
#include "Enums.h"
#include "Player.h"
#include "Ghost.h"
#include <chrono>

using namespace std::chrono;

class Game
{
    // player pointer
    Player* p_player = nullptr;

    // pointer to array of ghosts
    Ghost* p_ghosts = nullptr;

    // level pointer
    //Level* p_level = nullptr;
    
    // Game States
    bool game_on = false;
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

        // Player movement and input keys
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

        // delay defaults
        static constexpr int gobble_delay = 750; // wait in milliseconds
        static constexpr int player_beat_delay = 1000; // wait in milliseconds
        static constexpr int refresh_delay = 45; //milliseconds

        // constructors
        Game();
        Game(Player& player, Ghost* ghosts);
        /*Game(Player& player, Level& level);*/

        //destructors
        ~Game();

        bool IsGameOn();
        void SetGameOn(bool game_on);
        bool IsGameOver();
        void SetGameOver(bool game_over);
        bool PauseForGobble();
        void SetPauseForGobble(bool pause);
        bool PauseForPalyerDeath();
        void SetPauseForPlayerDeath(bool pause);
        int GetCurrentScene();
        void SetCurrentScene(int scene);
        void SetTotalScenes(int scenes);
        void NextScene();
        void SetPlaySFX(Play sfx);
        Play GetPlaySFX();
        void StartSFX();
        void GetKeyboardInput();
        void AddPlayer(Player& player);
        Player* GetPlayer();
        /*void AddLevel(Level& level);
        Level* GetLevel();*/
        void Add(Player& player, Ghost* ghosts);

};
