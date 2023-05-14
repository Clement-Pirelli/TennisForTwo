#pragma once
#include "Types.h"

namespace params
{
	//drawing
	constexpr Line tennisFloor = { { -150.0f, .0f}, { 150.0f, .0f} };
	constexpr Line tennisNet = { {.0f, 10.0f}, { .0f, 30.f} };

	//spatial
	constexpr vec2 spawnPosition = { .0f, 40.f };
	constexpr float boundsMargin = 20.0f;
	constexpr Rect bounds = { { tennisFloor.start.x() - boundsMargin, -100.0f }, { tennisFloor.end.x() + boundsMargin, 100.0f } };
	constexpr float spaceScale = 200.0f;

	//air resistance
	constexpr float airDensity = 1.2f;
	constexpr float drag = .5f;
	constexpr float ballArea = 1.0f;
	constexpr float airResistanceCoefficient = .001f;
	constexpr float airResistanceConstant = (airDensity * drag * ballArea / 2.0f) * airResistanceCoefficient;

	//ball behaviour
	constexpr float gravity = .006f;
}