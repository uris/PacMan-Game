#pragma once
#include "Character.h"

class Ghost: public Character
{
private:
    char ghost;
    char square_content_now;
    char square_content_prior;
    
    Coord spawn_target; // just above spawn area
    Coord roam_target; // roams to top right hand side
    Coord chase_modifier; // runs after player position
    
    Mode mode = Mode::CHASE;
    bool run_first_move = true;
    bool is_edible = false;
    bool color_on = false;
    int color = 455;
    int wait = 0;
    bool skip_turn = false; // use this to slow down moster if edible
    int look_ahead = 5; // how far ahead the IA looks for player

public:
    // constructors
    Ghost();
    Ghost(Ghosts ghost);

    // destructors
    ~Ghost();

    //methods
    char GetContentBeforeEntering();
    void SetContentBeforeEntering(char content);
    Mode GetMode();
    void SetMode(Mode mode);
    bool IsEdible();
    void SetEdible(bool edible);
    bool FlashBlue();
    void SetFlashBlue(bool flash);
    int GetColor();
    void SetColor(int color);
    int GetWait();
    void SetWait(int wait);
    void SetSkipTurn(bool skip_turn);
    bool SkipTurn();
    void SetLookAhead(int moves);
    int GetLookAhead();
    
};