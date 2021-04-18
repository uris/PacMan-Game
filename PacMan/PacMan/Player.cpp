#include "Game.h"

Player::Player() 
{

};

Player::~Player()
{
	p_game = nullptr;
}


// methods
void Player::AddLives(int number)
{
	lives += number;
}

void Player::TakeLives(int number)
{
	lives -= number;
}

void Player::ReSpawn()
{
	previous_position = spawn_position;
	current_position = spawn_position;
	current_direction = Direction::LEFT;
	previous_direction = Direction::LEFT;
}

void Player::EatGhost(int ghost_index)
{
	ghosts_eaten[ghost_index] = true;
}

void Player::EatGhost(char character)
{
	switch (character)
	{
	case 'R':
		ghosts_eaten[0];
		break;
	case 'Y':
		ghosts_eaten[1];
		break;
	case 'B':
		ghosts_eaten[2];
		break;
	case 'P':
		ghosts_eaten[3];
		break;

	}
}

void Player::ClearEatenGohsts()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		ghosts_eaten[i] = false;
	}
}

bool Player::AllGhostsEaten()
{
	for (int i = 0; i < sizeof(ghosts_eaten); i++)
	{
		if (ghosts_eaten[i] == false)
			return false;
	}
	return true;
}

bool Player::HasNoLives()
{
	return(lives < 1 ? true : false);
}

void Player::MovePlayer()
{
	// if the players current position is a teleport, do the teleport before calcing move
	Teleport();
		
	// set the prvious potisiton to the current position as we will move out of the current position
	previous_position = current_position;

	// assume the new position will move in the same direction as what was the previous direction
	Coord next_position = current_position + previous_direction;

	// if the user pressed a new direction key, we now need to process this new direction (which is the current direction)
	switch (current_direction)
	{
	case Direction::UP: // if up
		if (p_game->p_level->NotWall(current_position + Direction::UP, Direction::UP)) { // and player can move into that position
			next_position = current_position + Direction::UP; // set the new position to moveing in that direction
			previous_direction = Direction::UP; // and now set the previous direction to the same as the current direction
		}
		break;
	case Direction::RIGHT: // right
		if (p_game->p_level->NotWall(current_position + Direction::RIGHT, Direction::RIGHT)) {
			next_position = current_position + Direction::RIGHT;
			previous_direction = Direction::RIGHT;
		}
		break;
	case Direction::DOWN: // down
		if (p_game->p_level->NotWall(current_position + Direction::DOWN, Direction::DOWN)) {
			next_position = current_position + Direction::DOWN;
			previous_direction = Direction::DOWN;
		}
		break;
	case Direction::LEFT: // left
		if (p_game->p_level->NotWall(current_position + Direction::LEFT, Direction::LEFT)) {
			next_position = current_position + Direction::LEFT;
			previous_direction = Direction::LEFT;
		}
		break;
	}

	// since we have updated tje previous direciton to the user selcted new direction if possible
	// if the player is unable to move in the previous direction (which is the new direction now)
	switch (previous_direction)
	{
	case Direction::UP: // previous/current direction is up
		if (!p_game->p_level->NotWall({ current_position.row - 1, current_position.col }, Direction::UP)) // if player bumps into a wall
			next_position = current_position; // the next position is the same as current position (ie the player does not move)
		break;
	case Direction::RIGHT:
		if (!p_game->p_level->NotWall({ current_position.row, current_position.col + 1}, Direction::RIGHT))
			next_position = current_position;
		break;
	case Direction::DOWN:
		if (!p_game->p_level->NotWall({ current_position.row + 1, current_position.col }, Direction::DOWN))
			next_position = current_position;
		break;
	case Direction::LEFT:
		if (!p_game->p_level->NotWall({ current_position.row, current_position.col - 1}, Direction::LEFT))
			next_position = current_position;
		break;
	}

	// set the player curretn position to the next poition
	current_position = next_position;

}

void Player::UpdateMapAfterMove()
{
	p_game->p_level->p_map[previous_position.row][previous_position.col] = char(Globals::space);
}

bool Player::PlayerGhostCollision(int ghost_index)
{
	return (current_position == p_game->p_ghosts[ghost_index]->GetCurrentPosition());
}

bool Player::PlayerFruitCollision()
{
	return (current_position == p_game->p_fruit->GetCurrentPosition());
}

void Player::CoutPlayer()
{
	Draw::SetColor(Globals::cPLAYER);
	
	if (eat_ghost_animation)
	{
		player_flash ? Draw::SetColor(Globals::cPLAYERFLASH) : Draw::SetColor(Globals::cPLAYER);
		player_flash = !player_flash;
	}
	
	if (die_animation)
	{
		switch (previous_direction)
		{
		case Direction::UP:
			previous_direction = Direction::RIGHT;
			cout << char(Globals::pacman_up_open);
			break;
		case Direction::RIGHT:
			previous_direction = Direction::DOWN;
			cout << char(Globals::pacman_right_open);
			break;
		case Direction::DOWN:
			previous_direction = Direction::LEFT;
			cout << char(Globals::pacman_down_open);
			break;
		case Direction::LEFT:
			previous_direction = Direction::UP;
			cout << char(Globals::pacman_left_open);
			break;
		default:
			cout << char(Globals::pacman_left_open);
			break;
		}
	}
	else if (chomp) { // mouth closed
		
		switch (previous_direction)
		{
		case Direction::UP:
			cout << char(Globals::pacman_up_closed);
			break;
		case Direction::RIGHT:
			cout << char(Globals::pacman_right_closed);
			break;
		case Direction::DOWN:
			cout << char(Globals::pacman_down_closed);
			break;
		case Direction::LEFT:
			cout << char(Globals::pacman_left_closed);
			break;
		default:
			cout << char(Globals::pacman_left_closed);
			break;
		}
		if(!eat_ghost_animation && current_position != previous_position)
		{
			chomp = !chomp;
		}
		
	}
	else // mouth open
	{
		switch (previous_direction)
		{
		case Direction::UP:
			cout << char(Globals::pacman_up_open);
			break;
		case Direction::RIGHT:
			cout << char(Globals::pacman_right_open);
			break;
		case Direction::DOWN:
			cout << char(Globals::pacman_down_open);
			break;
		case Direction::LEFT:
			cout << char(Globals::pacman_left_open);
			break;
		default:
			cout << char(Globals::pacman_left_open);
			break;
		}
		if (!eat_ghost_animation && current_position != previous_position)
		{
			player_move_content == Globals::space ? chomp = false : chomp = !chomp;
		}
		
	}
}

void Player::SetScore(const int score)
{
	this->score = score;
}


// getters
int Player::Lives()
{
	return(lives);
}

int Player::GetScore()
{
	return(score);
}

char Player::GetMovedIntoSquareContents()
{
	return p_game->p_level->p_map[current_position.row][current_position.col];
}

Coord Player::GetCurrentPosition()
{
	return current_position;
}

bool Player::GameRefIsSet()
{
	return (p_game ? true : false);
}


// setters
void Player::SetLives()
{
	lives = lives;
}

void Player::SetLives(int number)
{
	lives = number;
}

void Player::SetGameRef(Game* p_game)
{
	this->p_game = p_game;
}

void Player::DeathAnimate(int g)
{
	die_animation = true;
	for (int i = 0; i < 48; i++)
	{
		p_game->p_level->DrawLevel(); //draw level directly bypassing all movement, etc..
	}
	die_animation = false;
	chomp = false;
}

void Player::EatGhostAnimate(int g, bool xtra_life)
{
	int iterations = xtra_life ? 10 : 5;
	eat_ghost_animation = true;
	for (int i = 0; i < iterations; i++)
	{
		p_game->p_level->DrawLevel(); //draw level directly bypassing all movement, etc..
		p_game->SetRefreshDelay();
	}
	eat_ghost_animation = false;
	player_flash = false;
}

void Player::SetEatGhostAnimate(const bool eat_ghost_animation)
{
	this->eat_ghost_animation = eat_ghost_animation;
}

void Player::SetDeathAnimation(const bool die_animation)
{
	this->die_animation = die_animation;
}

bool Player::GetEatGhostAnimate()
{
	return eat_ghost_animation;
}

bool Player::GetDieAnimate()
{
	return die_animation;
}

void Player::AddEatenFruit(const Fruits fruit)
{
	switch (fruit)
	{
	case Fruits::CHERRY:
		fruits_eaten[0] = fruits_eaten[0] + 1;
		break;
	case Fruits::STRAWBERRY:
		fruits_eaten[1] = fruits_eaten[1] + 1;
		break;
	case Fruits::APPLE:
		fruits_eaten[2] = fruits_eaten[2] + 1;
		break;
	case Fruits::PEAR:
		fruits_eaten[3] = fruits_eaten[3] + 1;
		break;
	}
}

void Player::CoutEatenFruits()
{
	string spaces = "";
	
	for (int i = 0; i < 4; i++)
	{
		spaces += (fruits_eaten[i] == 0 ? " " : "");
	}

	cout << spaces;

	if (fruits_eaten[0] > 0)
	{
		Draw::SetColor(Globals::cCHERRY);
		cout << char(Globals::fCherry);
	}

	if (fruits_eaten[1] > 0)
	{
		Draw::SetColor(Globals::cSTRAWBERRY);
		cout << char(Globals::fStrawberry);
	}

	if (fruits_eaten[2] > 0)
	{
		Draw::SetColor(Globals::cAPPLE);
		cout << char(Globals::fApple);
	}

	if (fruits_eaten[3] > 0)
	{
		Draw::SetColor(Globals::cPEAR);
		cout << char(Globals::fPear);
	}

	Draw::SetColor(7);
}

void Player::IncrementScore(const Object object_eaten)
{
	switch (object_eaten)
	{
	case Object::FRUIT:
		score += p_game->p_level->fruit_points;
		AddEatenFruit(p_game->p_fruit->FruitType());
		break;
	case Object::PELLET:
		score += p_game->p_level->points_pellet;
		p_game->p_level->eaten_pellets++;
		break;
	case Object::POWERUP:
		score += p_game->p_level->points_pellet;
		p_game->p_level->eaten_pellets++;
		break;
	case Object::GHOST:
		score += p_game->p_level->points_ghost;
		break;
	case Object::ALL_GHOSTS:
		score += p_game->p_level->all_ghost_bonus;
		break;
	default:
		score = score;
		break;
	}
}

void Player::ResetPlayer()
{
	lives = 3;
	score = 0;
	ClearEatenGohsts();
}