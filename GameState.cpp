#include "GameState.h"
#include <math.h>

namespace 
{
	bool outsideRange(float x, float min, float max)
	{
		return x < min || x > max;
	}

	bool differentSigns(float x, float y)
	{
		return signbit(x) != signbit(y);
	}
}

void GameState::updateBall(float deltaTime)
{
	const float airResistance = params::airResistanceConstant * ballVelocity.squaredLength();

	const vec2 acceleration = vec2(.0f, -params::gravity) - ballVelocity * airResistance;
	ballVelocity += acceleration * deltaTime;

	const vec2 oldPosition = ballPosition.load();
	vec2 newPosition = oldPosition + ballVelocity;

	if (newPosition.y() <= .0f)
	{
		newPosition.y() = .0f;
		ballVelocity.y() = -ballVelocity.y();
		bounces++;
	}

	const bool outsideBounds = outsideRange(newPosition.x(), params::bounds.bottomLeft.x(), params::bounds.topRight.x());
	if (bounces > 1 || outsideBounds)
	{
		newPosition = params::spawnPosition;
		ballVelocity = getNewVelocity();
		bounces = 0;
	}

	if (differentSigns(oldPosition.x(), newPosition.x()))
	{
		bounces = 0;
	}

	ballPosition.store(newPosition.clampedBy(params::bounds.bottomLeft, params::bounds.topRight));
}

bool GameState::handleKeyDown(SDL_Scancode scancode)
{
	switch (scancode)
	{
	case SDL_SCANCODE_ESCAPE:
		return false;
	case SDL_SCANCODE_1:
	{
		const Channels chans = channels.load();
		channels.store(Channels(chans ^ First));
	} break;
	case SDL_SCANCODE_2:
	{
		const Channels chans = channels.load();
		channels.store(Channels(chans ^ Second));
	} break;
	case SDL_SCANCODE_A:
	{
		const vec2 position = ballPosition.load();
		if (ballVelocity.x() > .0f && position.x() > .0f)
		{
			ballVelocity.x() *= -1.0f;
			ballVelocity.x() -= 4.0f;
			ballVelocity.y() = 5.0f;
		}
	} break;
	case SDL_SCANCODE_D:
	{
		const vec2 position = ballPosition.load();
		if (ballVelocity.x() < .0f && position.x() < .0f)
		{
			ballVelocity.x() *= -1.0f;
			ballVelocity.x() += 4.0f;
			ballVelocity.y() = 5.0f;
		}
	} break;
	}

	return true;
}

vec2 GameState::getNewVelocity()
{
	float newAngle = rnd::range(static_cast<float>(M_PI) / 6.0f, static_cast<float>(M_PI) / 3.0f);
	float newVelocity = rnd::range(3.0f, 5.0f);
	float axis = rnd::coinFlip() ? -1.0f : 1.0f;

	return vec2(cosf(newAngle) * axis, sinf(newAngle)) * newVelocity;
}
