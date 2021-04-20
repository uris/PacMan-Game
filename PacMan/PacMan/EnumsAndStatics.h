#pragma once

enum class Object {
    FRUIT = 1,
    PELLET = 2,
    POWERUP = 3,
    GHOST = 4,
    ALL_GHOSTS = 5,
    NONE = 0
};

enum class Resolution {
    TINY = 12,
    SMALL = 16,
    NORMAL = 20,
    LARGE = 24,
    EXTRA_LARGE = 28,
    HUGE = 36
};

enum class MenuTemplates {
    LIST_SCENES = 0,
    EDIT_SCENE_OPTIONS = 1,
    CHOOSE_EDIT_PLAY = 2,
    LIST_TEMPLATES = 3,
    DISPLAY_RESOLUTION = 4,
    NONE = 5
};

enum class Menu {
    EDIT = 0,
    OTHER = 1
};

enum class EditMode {
    EDIT = 0,
    HOVER = 1
};

enum class Direction {
    UP = 0,
    RIGHT = 1,
    DOWN = 2,
    LEFT = 3,
    NONE = -1
};

enum class Mode {
    CHASE = 0,
    ROAM = 1,
    RUN = 2,
    SPAWN = -1
};

enum class Play {
    MUNCH = 6,
    LG_MUNCH = 7,
    EAT_GHOST = 3,
    INTRO = 5,
    SIREN = 8,
    LIFE = 11,
    INTERMISSION = 4,
    EAT_FRUIT = 2,
    DEATH = 1,
    POWER_UP = 10,
    BONUS = 0,
    CREDIT = 12,
    NONE = -1
};

enum class Ghosts
{
    RED = 0,
    YELLOW = 1,
    BLUE = 2,
    PINK = 3,
    NONE = -1,
};

enum class Fruits
{
    CHERRY = 0,
    STRAWBERRY = 1,
    APPLE = 2,
    PEAR = 4,
    NONE = -1,
};

struct Globals
{
    // Fruis
    static constexpr int fCherry = 199;
    static constexpr int fStrawberry = 189;
    static constexpr int fApple = 208;
    static constexpr int fPear = 211;
    static constexpr int fSpawnRate = 60;
    static constexpr int fDelay = 1;
    
    // Game Colors
    static constexpr int cINVISIBLE = 0; // black on black
    static constexpr int cWHITE = 7; // white
    static constexpr int cPLAYER = 14; // yellow
    static constexpr int cPLAYERFLASH = 9; // yellow
    static constexpr int cWALLS = 392; // gray on gray text
    static constexpr int cGHOST_ON = 1; // for ghost flash
    static constexpr int cGHOST_OFF = 9; // for ghost flash
    static constexpr int cIMMOVABLE = 16;
    static constexpr int cEDITABLE = 9;
    static constexpr int cRED = 4;
    static constexpr int cYELLOW = 6;
    static constexpr int cBLUE = 13;
    static constexpr int cPINK = 10;
    static constexpr int cPELLETS = 6;
    static constexpr int cWALLS2 = 9;
    static constexpr int cCHERRY = 12;
    static constexpr int cSTRAWBERRY = 12;
    static constexpr int cAPPLE = 10;
    static constexpr int cPEAR = 6;

    // Editor menu colors
    static constexpr int c_blackwhite = 7; // black bg, white text
    static constexpr int c_bluewhite = 30; // blue bg, white text
    static constexpr int c_whiteblack = 240; // white bg, black text
    static constexpr int c_redwhite = 335; // white bg, black text

    /// Editor settings
    static constexpr int editor_refresh_delay = 0; //milliseconds
    static constexpr int cursor = 240;
    static constexpr int max_scene_rows = 37;
    static constexpr int min_scene_rows = 10;
    static constexpr int max_scene_cols = 77;
    static constexpr int min_scene_cols = 17;

    //pacman
    static constexpr int pacman_left_open = 1;
    static constexpr int pacman_right_open = 2;
    static constexpr int pacman_up_open = 11;
    static constexpr int pacman_down_open = 12;
    static constexpr int pacman_left_closed = 3;
    static constexpr int pacman_right_closed = 4;
    static constexpr int pacman_up_closed = 5;
    static constexpr int pacman_down_closed = 6;

    // Ghosts
    static constexpr int gRED = 0;
    static constexpr int gYELLOW = 1;
    static constexpr int gBLUE = 2;
    static constexpr int gPINK = 3;
    static constexpr int look_ahead = 4; // how far ahead the IA looks for player
    static constexpr int total_ghosts = 4;
    static constexpr int ghost_run = 186;
    static constexpr int ghost_down = 206;
    static constexpr int ghost_up = 202;
    static constexpr int ghost_left = 204;
    static constexpr int ghost_right = 185;
    static constexpr int spawn_delay = 20;
    static constexpr int spawn_stagger = 20;

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
    static constexpr char fruit = 'F'; // pellet ascii
    static constexpr char corner_marker = '+'; // pellet ascii

    // long walls
    static constexpr int lwall_184 = 184; // long wall up on left
    static constexpr int lwall_213 = 213; // long wall up on right
    static constexpr int lwall_212 = 212; // long wall across on top
    static constexpr int lwall_209 = 209; // long wall across on bottom
    static constexpr int lwall_187 = 187; // long wall top left corner
    static constexpr int lwall_203 = 203; // long wall top right corner
    static constexpr int lwall_188 = 188; // long wall bottom left corner
    static constexpr int lwall_200 = 200; // long wall bottom right corner
    static constexpr int lwall_155 = 155; // long wall bottom right cap
    static constexpr int lwall_210 = 210; // long wall top left cap
    static constexpr int lwall_183 = 183; // long wall bottom left cap
    static constexpr int lwall_214 = 214; // long wall bottom right cap
    static constexpr int lwall_220 = 220; // Connector left
    static constexpr int lwall_221 = 221; // Connector Up
    static constexpr int lwall_222 = 222; // Connector right
    static constexpr int lwall_223 = 223; // Connector down

    // short walls
    static constexpr int lwall_180 = 180; // Top Cap
    static constexpr int lwall_192 = 192; // Right cap
    static constexpr int lwall_197 = 197; // Bottom Cap
    static constexpr int lwall_217 = 217; // Left Cap
    static constexpr int lwall_193 = 193; // Horizontal Straight
    static constexpr int lwall_195 = 195; // Vertical Straight
    static constexpr int lwall_191 = 191; // bottom T
    static constexpr int lwall_194 = 194; // Top T
    static constexpr int lwall_196 = 196; // Left T
    static constexpr int lwall_218 = 218; // Right T
    static constexpr int lwall_179 = 179; // Intersection
    static constexpr int lwall_190 = 190; // Island
    static constexpr int lwall_181 = 181; // Bottom Right Corner
    static constexpr int lwall_198 = 198; // Top Left Corner
    static constexpr int lwall_207 = 207; // Top Right Corner
    static constexpr int lwall_216 = 216; // Bottom Left Corner
    static constexpr int lwall_201 = 201; // Inner Bottom Right Corner
    static constexpr int lwall_205 = 205; // Inner Top Left Corner
    static constexpr int lwall_215 = 215; // Inner Bottom Left Corner
    static constexpr int lwall_182 = 182; // Inner Top Right  Corner

    // Movement and input keys
    static constexpr char kLEFT = 97; // 'a';
    static constexpr char kUP = 119; // 'w';
    static constexpr char kDOWN = 115; // 's';
    static constexpr char kRIGHT = 100; // 'd';
    static constexpr char kF = 102; // 'f';
    static constexpr char kARROW_UP = 72; // arrow up
    static constexpr char kARROW_DOWN = 80; // arrow down
    static constexpr char kARROW_LEFT = 75; // arrow left
    static constexpr char kARROW_RIGHT = 77; // arrow right
    static constexpr char kESCAPE = 27; // escape
    static constexpr char kYES = 121; // 'y'
    static constexpr char kNO = 110; // 'n'
    static constexpr char kRETURN = 13; // return
    static constexpr char kEQUALS = 61; // equals key
    static constexpr char kMINUS = 45; // minus key
    static constexpr char kSPACE = 32; // space key
    static constexpr char kAT = 64; // @
    static constexpr char kHASH = 35; // @
    static constexpr char kN = 110; // 'n'

    // Game defaults
    static constexpr int gobble_delay = 750; // wait in milliseconds
    static constexpr int player_beat_delay = 1000; // wait in milliseconds
    static constexpr int refresh_delay_controller = 65; //milliseconds
    static constexpr int refresh_delay = 20; //milliseconds
    static constexpr int base_extra_lives_points = 10350; //10350 = all pellets + 1 fruit + 3*all ghosts
    static constexpr float extra_life_multiplier = 1.5f;
}; 