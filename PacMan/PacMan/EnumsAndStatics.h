#pragma once

enum class Resolution {
    TINY = 12,
    SMALL = 16,
    DEFAULT, NORMAL = 20,
    LARGE = 24,
    EXTRA_LARGE = 28,
    HUGE = 36
};

enum class MenuTemplates {
    LIST_SCENES = 0,
    EDIT_SCENE_OPTIONS = 1,
    CHOOSE_EDIT_PLAY = 2,
    LIST_TEMPLATES = 3,
    NONE = 4,
};

enum class Menu {
    EDIT = 0,
    OTHER = 1,
};

enum class EditMode {
    EDIT = 0,
    HOVER = 1,
};

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
    // Game Colors
    static constexpr int cINVISIBLE = 0; // black on black
    static constexpr int cWHITE = 7; // white
    static constexpr int cPLAYER = 14; // yellow
    static constexpr int cWALLS = 392; // gray on gray text
    //static constexpr int cGHOST_ON = 275; // for ghost flash
    //static constexpr int cGHOST_OFF = 155; // for ghost flash
    static constexpr int cGHOST_ON = 7; // for ghost flash
    static constexpr int cGHOST_OFF = 9; // for ghost flash
    /*static constexpr int cRED = 71;
    static constexpr int cYELLOW = 367;
    static constexpr int cBLUE = 435;
    static constexpr int cPINK = 479;*/
    static constexpr int cRED = 4;
    static constexpr int cYELLOW = 6;
    static constexpr int cBLUE = 13;
    static constexpr int cPINK = 10;
    static constexpr int cPELLETS = 6;

    // Editor menu colors
    static constexpr int c_blackwhite = 7; // black bg, white text
    static constexpr int c_bluewhite = 30; // blue bg, white text
    static constexpr int c_whiteblack = 240; // white bg, black text
    static constexpr int c_redwhite = 335; // white bg, black text

    /// Editor settings
    static constexpr int editor_refresh_delay = 25; //milliseconds
    static constexpr int cursor = 240;
    static constexpr int max_scene_rows = 37;
    static constexpr int min_scene_rows = 10;
    static constexpr int max_scene_cols = 77;
    static constexpr int min_scene_cols = 17;


    // Ghosts
    static constexpr int gRED = 0;
    static constexpr int gYELLOW = 1;
    static constexpr int gBLUE = 2;
    static constexpr int gPINK = 3;
    static constexpr int look_ahead = 4; // how far ahead the IA looks for player
    static constexpr int total_ghosts = 4;

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
    static constexpr int fruit = 245; // pellet ascii

    static constexpr int pacman_left_open = 1;
    static constexpr int pacman_right_open = 2;
    static constexpr int pacman_up_open = 11;
    static constexpr int pacman_down_open = 12;
    static constexpr int pacman_left_closed = 3;
    static constexpr int pacman_right_closed = 4;
    static constexpr int pacman_up_closed = 5;
    static constexpr int pacman_down_closed = 6;
    static constexpr int ghost_run = 186;
    static constexpr int ghost_down = 206;
    static constexpr int ghost_up = 185;
    static constexpr int ghost_left = 204;
    static constexpr int ghost_right = 202;

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
    static constexpr char kRETURN = 13;
    static constexpr char kSPACE = 32;
    static constexpr char kN = 110; // 'n'

    // Game delay / speed defaults
    static constexpr int gobble_delay = 750; // wait in milliseconds
    static constexpr int player_beat_delay = 1000; // wait in milliseconds
    static constexpr int refresh_delay = 0; //milliseconds
}; 