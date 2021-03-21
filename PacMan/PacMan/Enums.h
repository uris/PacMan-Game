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