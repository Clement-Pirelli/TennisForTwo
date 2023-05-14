#pragma once

#include "vec.h"
#include "params.h"
#include "types.h"
#include "rand.h"
#include <atomic>
#include <SDL/SDL.h>

struct GameState
{
	std::atomic<Channels> channels = All;
	std::atomic<unsigned long long> audioTick = 0ull;

	std::atomic<vec2> ballPosition = params::spawnPosition;
	vec2 ballVelocity = getNewVelocity();

	int bounces = 0;

	void updateBall(float deltaTime);

	bool handleKeyDown(SDL_Scancode scancode);

	static vec2 getNewVelocity();
};