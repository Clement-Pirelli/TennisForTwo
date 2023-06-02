#include "GameState.h"
#include <math.h>
#include <optional>
#include "Collisions.h"

namespace 
{
	[[nodiscard]]
	bool outsideRange(float x, float min, float max)
	{
		return x < min || x > max;
	}

	[[nodiscard]]
	bool differentSigns(float x, float y)
	{
		return signbit(x) != signbit(y);
	}

	[[nodiscard]]
	vec2 accelerate(vec2 velocity, float deltaTime)
	{
		const float airResistance = params::airResistanceConstant * velocity.squaredLength();
		const vec2 acceleration = vec2(.0f, -params::gravity) - velocity * airResistance;
		return velocity + acceleration * deltaTime;
	}

	void handleFloorHit(vec2& newPosition, vec2& ballVelocity, PlayingState& s)
	{
		if (newPosition.y() <= .0f)
		{
			newPosition.y() = .0f;
			ballVelocity.y() = -ballVelocity.y() * params::floorAbsorbtion;
			s.bounces++;
		}
	}

	[[nodiscard]]
	std::optional<Player> getPlayerLost(vec2 position, PlayingState state)
	{
		//only one bounce allowed
		if (state.bounces > 1)
		{
			return position.x() < 0.0f ? LeftPlayer : RightPlayer;
		}
		//serving outside of bounds gives a point to the receiving side
		else if (outsideRange(position.x(), params::bounds.bottomLeft.x(), params::bounds.topRight.x()))
		{
			return position.x() < 0.0f ? RightPlayer : LeftPlayer;
		}

		return std::nullopt;
	}

	template<typename... Ts>
	struct overloaded : Ts... { using Ts::operator()...; };

	template<typename... Ts>
	overloaded(Ts...) -> overloaded<Ts...>;
}

bool Game::onBallHit(Player player)
{
	PlayerState& state = players[player];
	if(state.alreadyHit)
	{
		return false;
	}

	ballVelocity.x() *= -1.0f;

	const vec2 input = getPlayerInput(player);
	ballVelocity.y() = input.y();
	//+= to preserve the bounce of the last hit
	ballVelocity.x() += input.x();

	state.strength = .0f;
	state.alreadyHit = true;
	return true;
}

void Game::update(float deltaTime)
{
	for(PlayerState& player : players)
	{
		player.increaseStrength(deltaTime);
		player.incrementAngle(deltaTime);
	}

	std::optional<States> newState;
	std::visit(overloaded{
		[&](ServingState s) {
			ballVelocity = {};
			ballPosition = params::playerParams[s.servingPlayer].spawnPosition;
		}, 
		[&, deltaTime](PlayingState& s) {
			
			ballVelocity = accelerate(ballVelocity, deltaTime);

			const vec2 oldPosition = ballPosition;
			vec2 newPosition = oldPosition + ballVelocity;
			
			//bounce on hitting the floor
			handleFloorHit(newPosition, ballVelocity, s);

			//bounce on the net
			if (const auto collision = collideWith(params::tennisNetCollision, ballVelocity, ballPosition))
			{
				ballVelocity = collision->velocity * params::netAbsorbtion;
				newPosition = collision->intersection + ballVelocity;
			}

			//bounce handling
			if (std::optional<Player> lost = getPlayerLost(newPosition, s))
			{
				const Player scoringPlayer = otherPlayer(*lost);
				newState = onScore(scoringPlayer, ScoreKind::NoFoul);
			}

			//reset bounces and alreadyHit if the ball crosses to the other field
			if (differentSigns(oldPosition.x(), newPosition.x()))
			{
				s.bounces = 0;
				for(PlayerState& player : players)
				{
					player.alreadyHit = false;
				}
			}

			ballPosition = newPosition.clampedBy(params::bounds.bottomLeft, params::bounds.topRight);
		}, 
		[&](WaitingForStart) { 
			ballPosition = params::tennisNet.start;
			ballVelocity = {};
		}
		}, gameState);

	if (newState.has_value()) 
	{
		gameState = newState.value();
	}
}

bool Game::handleKeyDown(SDL_Scancode scancode)
{
	switch (scancode)
	{
	case SDL_SCANCODE_ESCAPE:
		return false;

	//these two are for debugging oscilloscope output
	case SDL_SCANCODE_1:
	{
		channels = Channels(channels ^ First);
	} break;
	case SDL_SCANCODE_2:
	{
		channels = Channels(channels ^ Second);
	} break;
	default:
		handlePlayerInput<LeftPlayer>(scancode, KeyState::Down);
		handlePlayerInput<RightPlayer>(scancode, KeyState::Down);
		break;
	}

	if(std::holds_alternative<WaitingForStart>(gameState))
	{
		if (players[LeftPlayer].strengthIncreasing && players[RightPlayer].strengthIncreasing)
		{
			for (PlayerState& player : players)
			{
				player.reset();
			}
			gameState = ServingState{ .servingPlayer = rnd::coinFlip() ? LeftPlayer : RightPlayer };
		}
	}

	return true;
}

void Game::handleKeyUp(SDL_Scancode scancode)
{
	handlePlayerInput<LeftPlayer>(scancode, KeyState::Up);
	handlePlayerInput<RightPlayer>(scancode, KeyState::Up);

	std::optional<States> newState;
	std::visit(overloaded{
	[&](ServingState s)
	{
		if (s.served(scancode))
		{
			onBallHit(s.servingPlayer);
			newState = PlayingState{};
		}
	},
	[&, scancode](const PlayingState state)
	{
		const bool rightPlayerHitting = scancode == params::playerParams[RightPlayer].hit && ballVelocity.x() > .0f && ballPosition.x() > .0f;
		const bool leftPlayerHitting = scancode == params::playerParams[LeftPlayer].hit && ballVelocity.x() < .0f && ballPosition.x() < .0f;
		if (rightPlayerHitting || leftPlayerHitting)
		{
			const Player playerHitting = leftPlayerHitting ? LeftPlayer : RightPlayer;
			const bool hitBeforeFirstBounce = state.bounces == 0;
			if (hitBeforeFirstBounce || !onBallHit(playerHitting))
			{
				const Player scoringPlayer = otherPlayer(playerHitting);
				newState = onScore(scoringPlayer, ScoreKind::Foul);
			}
		}
	},
	[](auto) {} }, gameState);

	if (newState.has_value())
	{
		gameState = newState.value();
	}
}

vec2 Game::getPlayerInput(Player p) const
{
	const PlayerState& state = players[p];
	const float mirror = (p == LeftPlayer) ? 1.0f : -1.0f;
	const float angle = state.angle + params::minAngle;
	const float strength = state.strength + params::minStrength;
	return vec2{ mirror * cosf(angle), sinf(angle) } * strength;
}

bool Game::checkWin() const
{
	const int leftScore = players[LeftPlayer].score;
	const int rightScore = players[RightPlayer].score;

	//best out of n
	if(abs(leftScore - rightScore) <= 1)
	{
		return false;
	}

	if (leftScore >= params::winScore && leftScore > rightScore)
	{
		SDL_Log("Left player won with a score of %i vs %i!", leftScore, rightScore);
		return true;
	}
	
	if(rightScore >= params::winScore && rightScore > leftScore)
	{
		SDL_Log("Right player won with a score of %i vs %i!", rightScore, leftScore);
		return true;
	}

	return false;
}

Game::States Game::onScore(Player scoringPlayer, ScoreKind kind)
{
	players[scoringPlayer].score++;

	for (PlayerState& player : players)
	{
		player.strength = params::minStrength;
		player.alreadyHit = false;
	}

	if (checkWin())
	{
		return WaitingForStart{};
	}
	else
	{
		SDL_Log("Score! The score is now %i vs %i!", players[LeftPlayer].score, players[RightPlayer].score);
		return ServingState{ .servingPlayer = kind == ScoreKind::Foul ? scoringPlayer : otherPlayer(scoringPlayer) };
	}
}

template<Player p>
void Game::handlePlayerInput(SDL_Scancode scancode, KeyState state)
{
	constexpr params::PlayerParams params = params::playerParams[p];
	const bool down = state == KeyState::Down;
	switch(scancode)
	{
	case params.up:
		players[p].angleIncreasing = down;
		break;
	case params.down:
		players[p].angleDecreasing = down;
		break;
	case params.hit:
		players[p].strengthIncreasing = down;
		break;
	}
}

void PlayerState::incrementAngle(float deltaTime)
{
	const int angleIncrement = (angleIncreasing ? 1 : 0) + (angleDecreasing ? -1 : 0);
	const float change = static_cast<float>(angleIncrement) * deltaTime * params::angleIncreaseRate;
	angle = std::max(std::min(angle + change, params::maxAngle - params::minAngle), params::minAngle);
}

void PlayerState::increaseStrength(float deltaTime)
{
	if(strengthIncreasing)
	{
		const float change = strength + deltaTime * params::strengthIncreaseRate;
		strength = std::min(change, params::maxStrength - params::minStrength);
	}
	else
	{
		strength = .0f;
	}
}