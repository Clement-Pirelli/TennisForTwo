#pragma once
#include "Types.h"
#include <SDL/SDL_scancode.h>
#include <array>

namespace params
{
	//spatial
	constexpr Line tennisFloor = { { -150.0f, .0f}, { 150.0f, .0f} };
	constexpr Line tennisNet = { {.0f, 10.0f}, { .0f, 30.f} };
	constexpr Line tennisNetCollision = { { tennisNet.start.x(), tennisFloor.start.y() }, tennisNet.end };
	constexpr float boundsMargin = 20.0f;
	constexpr Rect bounds = { { tennisFloor.start.x() - boundsMargin, -100.0f }, { tennisFloor.end.x() + boundsMargin, 100.0f } };
	constexpr float spaceScale = 200.0f;

	//UI
	constexpr float inputUILengthFactor = 5.0f;

	//physics
	constexpr float airDensity = 1.2f;
	constexpr float drag = .5f;
	constexpr float ballArea = 1.0f;
	constexpr float airResistanceCoefficient = .001f;
	constexpr float airResistanceConstant = (airDensity * drag * ballArea / 2.0f) * airResistanceCoefficient;

	constexpr float gravity = .006f;
	constexpr float floorAbsorbtion = .9f;
	constexpr vec2 netAbsorbtion = { 1.0f, .5f };

	struct PlayerParams
	{
		vec2 spawnPosition;
		vec2 inputUIPos;
		SDL_Scancode up;
		SDL_Scancode down;
		SDL_Scancode hit;
		
	};

	constexpr std::array<PlayerParams, 2> playerParams =
	{
		PlayerParams
		{
			.spawnPosition = { -100.0f, 40.f },
			.inputUIPos = tennisFloor.start - vec2(.0f, 25.0f),
			.up = SDL_SCANCODE_W,
			.down = SDL_SCANCODE_S,
			.hit = SDL_SCANCODE_D
		},
		PlayerParams
		{
			.spawnPosition = { 100.0f, 40.0f },
			.inputUIPos = tennisFloor.end + vec2(.0f, -25.0f),
			.up = SDL_SCANCODE_UP,
			.down = SDL_SCANCODE_DOWN,
			.hit = SDL_SCANCODE_LEFT
		}
	};

	constexpr float angleIncreaseRate = .001f;
	constexpr float minAngle = .0f;
	constexpr float maxAngle = .333f * 3.14f;
	constexpr float startingAngle = .12f * 3.14f;

	constexpr float strengthIncreaseRate = .01f;
	constexpr float minStrength = .8f;
	constexpr float maxStrength = 5.0f;

	//game rules
	constexpr int winScore = 3;
}