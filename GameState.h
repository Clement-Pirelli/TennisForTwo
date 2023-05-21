#pragma once

#include "vec.h"
#include "params.h"
#include "types.h"
#include "rand.h"
#include <atomic>
#include <SDL/SDL.h>
#include <variant>
#include <array>
#include "Timer.h"

struct PlayerState
{
	float angle = params::startingAngle;
	float strength{};
	int score{};
	bool alreadyHit {};

	void reset() 
	{ 
		angle = params::startingAngle;
		strength = {};
		score = {};
		alreadyHit = {};
	}

	void incrementAngle(float deltaTime);
	void increaseStrength(float deltaTime);

	bool angleIncreasing {};
	bool angleDecreasing {};
	bool strengthIncreasing {};
};

struct ServingState
{
	Player servingPlayer;
	bool served(SDL_Scancode scanCode) const { return scanCode == params::playerParams[servingPlayer].hit; }
};

struct PlayingState
{
	int bounces = 0;
};

struct WaitingForStart {};

class Game
{
public:
	void update(float deltaTime);
	bool handleKeyDown(SDL_Scancode scancode);
	void handleKeyUp(SDL_Scancode scancode);

	Channels channels = All;
	vec2 getBallPosition() const { return ballPosition; }
	vec2 getPlayerInput(Player p) const;
private:

	using States = std::variant<ServingState, PlayingState, WaitingForStart>;

	bool onBallHit(Player player);
	enum class KeyState { Up, Down };
	template<Player p>
	void handlePlayerInput(SDL_Scancode scancode, KeyState key);

	[[nodiscard]]
	bool checkWin() const;
	enum class ScoreKind{ Foul, NoFoul };
	States onScore(Player servingPlayer, ScoreKind kind);

	std::array<PlayerState, 2> players;

	vec2 ballPosition;
	vec2 ballVelocity;

	States gameState = WaitingForStart{};
};