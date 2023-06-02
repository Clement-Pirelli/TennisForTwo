#pragma once

#include "vec.h"

enum Channels {
	None = 0,
	First = 1,
	Second = 2,
	All = First | Second
};

enum class Axis : bool
{
	X,
	Y
};

enum Player 
{
	RightPlayer,
	LeftPlayer
};

constexpr Player otherPlayer(Player p) { return p == RightPlayer ? LeftPlayer : RightPlayer; }

struct Line
{
	vec2 start;
	vec2 end;

	vec2 draw(float sample) const
	{
		return vec2::lerp(start, end, sample);
	}
};

struct Rect
{
	vec2 bottomLeft;
	vec2 topRight;
};