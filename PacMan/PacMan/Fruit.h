#pragma once
#include "Character.h"
#include "EnumsAndStatics.h"

class Fruit : public Character
{
    // forward class declared as pointer to game object
    class Game* p_game = nullptr;

private:
    // gets set once
    char fruit = Globals::fruit; // the fruit character
    int color = 71; // fruit color
    bool not_eaten = true; // will continue to spawn until eaten
    
    int max_spawns = 2; // max number of spawns for the fruit
    bool is_in_level = false; // state of fruit in scene
    Fruits fruit_type = Fruits::NONE;
    bool skip_turn = false;
    int wait = Globals::fDelay;
    

    // movement
    int move_count = 0; // number of moves the fruit has made
    int max_moves = 30; // the max number of moves before fruit begins to go towards the closest tp
    Coord exit_target; // moves randomly untill it needs to exit - get set based on level teleport
    
    // changes as the game / level changes
    char square_content_now = ' ';
    char square_content_prior = ' ';

public:
    int spawn_count = 0; // number of times the fruit has spawned
    int ticks = 0;
    // constructors
    Fruit();
    Fruit(Fruits fruit);

    // destructors
    ~Fruit();

    // methods
    void SpawnFruit();
    void KillFruit(const bool player_died = false);
    void MoveFruit(const Direction direction);
    Direction RandomFruitMove();
    int MakeFruitMove();
    /*char FruitContentNow(Direction best_move);*/
    void Teleport(Coord& fruit_position, Direction& fruit_direction);
    void CoutFruit();
    void ResetFruit();

    // Getters
    Coord GetExitTarget();
    char GetPreviousSqaureContent();
    char GetContentCurrent();
    int GetColor();
    Fruits FruitType();
    bool GameRefIsSet();
    bool FruitActive();

    // Setters
    void SetPreviousSqaureContent(char content);
    void SetContentCurrent(char content);
    void SetContentCurrent(bool same);
    void SetGameRef(Game* p_game);
    void SetFruitType(Fruits fruit_type);
    void SetFruitEaten();
};