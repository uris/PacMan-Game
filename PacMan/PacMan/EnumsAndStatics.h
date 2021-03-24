#pragma once

enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE = -1,
};

enum class Mode {
    CHASE = 0,
    ROAM = 1,
    RUN = 2,
    SPAWN = -1,
};

enum class Play {
    MUNCH,
    EAT_GHOST,
    INTRO,
    SIREN,
    LIFE,
    INTERMISSION,
    EAT_FRUIT,
    DEATH,
    CREDIT,
    POWER_UP,
    NONE,
};

enum class Ghosts
{
    RED = 0,
    YELLOW = 1,
    BLUE = 2,
    PINK = 3,
};

struct Globals
{
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
    static constexpr int look_ahead = 4; // how far ahead the IA looks for player

    // Level Map
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
    static constexpr char player = 'C'; // pellet ascii

    // Movement and input keys
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

    // Game delay / speed defaults
    static constexpr int gobble_delay = 750; // wait in milliseconds
    static constexpr int player_beat_delay = 1000; // wait in milliseconds
    static constexpr int refresh_delay = 100; //milliseconds
};