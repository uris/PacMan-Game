#pragma once
#include <chrono>
#include <string>
#include "Coord.h"
#include "Ghost.h"
#include "Player.h"

using namespace std;
using namespace std::chrono;

class Level
{
public:
    // Colors
    static constexpr int cINVISIBLE = 0; // black on black
    static constexpr int cWHITE = 7; // white
    static constexpr int cPLAYER = 14; // yellow
    static constexpr int cWALLS = 392; // gray on gray text
    static constexpr int cGHOST_ON = 275; // for ghost flash
    static constexpr int cGHOST_OFF = 155; // for ghost flash

    // Ghosts
    static constexpr int gRED = 0;
    static constexpr int gYELLOW = 1;
    static constexpr int gBLUE = 2;
    static constexpr int gPINK = 3;

    // level chars
    static constexpr int powerup = 254; //extended set of ascii 
    static constexpr int pellet = 250; // extended set of ascii
    static constexpr char player_start = 'S'; // pellet ascii
    static constexpr char ghost_spawn_target = '^'; // pellet ascii
    static constexpr char space = ' '; // pellet ascii
    static constexpr char invisible_wall = '%'; // pellet ascii
    static constexpr char one_way = '$'; // pellet ascii
    static constexpr char teleport = 'T'; // pellet ascii
    static constexpr char red_ghost = 'R'; // pellet ascii
    static constexpr char yellow_ghost = 'Y'; // pellet ascii
    static constexpr char blue_ghost = 'B'; // pellet ascii
    static constexpr char pink_ghost = 'P'; // pellet ascii

    // level map and scene
    string title = "Scene 1";
    char** p_map = nullptr;
    int rows = 23;
    int cols = 47;
    bool level_paused = true;
    Coord ghost_spawn;

    // teleport coords and direction
    Coord tp_1, tp_2;
    bool tp_row = true;

    // level stats
    int total_pellets = 0; // totala level pellets
    int eaten_pellets = 0; // pellets consumed
    int eaten_ghosts = 0; // pellets consumed
    int all_eaten_ghosts = 0; // pellets consumed
    int points_pellet = 10;
    int points_ghost = 250;
    int all_ghost_bonus = 2600;
    bool is_complete = false;

    // Manange modes
    // bool ghosts_in_a_row[4]{false,false,false,false};
    Mode level_mode = Mode::CHASE;
    int edible_ghost_duration = 20; // seconds
    int roam_count = 8;
    int roam_for = 6; // seconds
    int chase_for = 16; // seconds
    int run_for = 15; // seconds
    high_resolution_clock::time_point roam_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point chase_time_start = chrono::high_resolution_clock::now();
    high_resolution_clock::time_point run_time_start = chrono::high_resolution_clock::now();
    duration<double> roam_duration = duration_cast<duration<double>>(roam_time_start - chrono::high_resolution_clock::now());
    duration<double> chase_duration = duration_cast<duration<double>>(chase_time_start - chrono::high_resolution_clock::now());
    duration<double> run_duration = duration_cast<duration<double>>(run_time_start - chrono::high_resolution_clock::now());


	//constructors
	Level();

	// destructors
	~Level();

	// methods
    void SetupLevel(Player* p_player, Ghost* p_ghosts, int& current_scene);
    void CreateLevelScene(Player* p_player, Ghost* p_ghosts, int& current_scene);
    string LoadSceneFromFile(string filename, int scene_to_load);
    Coord MapSize(const string& map);
    void DrawLevel(Player* p_player, Ghost* p_ghosts);
    bool NotWall(const Player* player, const Coord& move, const Direction& direction);
    void CheckLevelComplete();
    bool IsTeleport(const Coord& move);
};