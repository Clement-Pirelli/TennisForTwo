#pragma once
#include "types.h"
#include <optional>

struct CollisionResult
{
	vec2 velocity;
	vec2 intersection;
};

[[nodiscard]]
std::optional<CollisionResult> collideWith(Line line, vec2 velocity, vec2 position);