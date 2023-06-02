#include <SDL/SDL.h>
#include "SDLUtils.h"
#include "scope_exit.h"
#include "vec.h"
#include "Timer.h"
#include "GameState.h"

std::atomic<unsigned long long> audioTick = 0;

//audio thread only ever loads, input/simulation thread loads then stores
std::atomic<Game> game;

void audioCallback(void*, Uint8* stream, int len)
{
	const Game gameCopy = game.load();
	const Channels chans = gameCopy.channels;

	float* floatStream = reinterpret_cast<float*>(stream);
	const int floatLen = len / 4;

	unsigned long long currentTick = audioTick.fetch_add(1) % frequency;

	auto drawPlayerInput = [&](Player p, float sample)
	{
		const vec2 input = gameCopy.getPlayerInput(p) * params::inputUILengthFactor;
		const vec2 inputPos = params::playerParams[p].inputUIPos;
		return Line(inputPos, inputPos + input).draw(sample);
	};

	for(int i = 0; i < floatLen; i+=2)
	{
		const float sample = static_cast<float>(i) / static_cast<float>(floatLen);
		
		const vec2 drawn = [&] {
			switch(currentTick % 5)
			{
			case 0:
				return params::tennisFloor.draw(sample);
			case 1:
				return params::tennisNet.draw(sample);
			case 2:
				return gameCopy.getBallPosition();
			case 3:
			{
				return drawPlayerInput(LeftPlayer, sample);
			}
			case 4:
			{
				return drawPlayerInput(RightPlayer, sample);
			}
			default:
				return vec2{};
			}
		}() / params::spaceScale;

		floatStream[i] = (chans & First) ? drawn.x() : .0f;
		floatStream[i + 1] = (chans & Second) ? drawn.y() : .0f;
	}
}


int main(int, char**)
{
	ENSURE(SDL_Init(SDL_INIT_TIMER | SDL_INIT_AUDIO | SDL_INIT_VIDEO) >= 0, "Welcome!", "SDL init failed");
	DEFER
	{
		SDL_Quit();
	};

	SDL_Window* window = SDL_CreateWindow("", 0, 0, 100, 100, SDL_WINDOW_ALWAYS_ON_TOP | SDL_WINDOW_INPUT_FOCUS);
	ENSURE(window != nullptr, "Window creation succeeded!", "SDL window creation failed");
	DEFER
	{
		SDL_DestroyWindow(window);
	};

	const auto [deviceId, spec] = utils::OpenAudioDevice(&audioCallback);
	ENSURE(deviceId != 0, "Audio device opened!", "Audio Device failed to open!");
	DEFER
	{
		SDL_CloseAudioDevice(deviceId);
	};
	
	SDL_PauseAudioDevice(deviceId, 0);

	Time previousTime = Time::now();
	SDL_Event sdlEvent;

	do
	{
		Game gameCopy = game.load();
		const Time currentTime = Time::now();
		float deltaTime = static_cast<float>((currentTime - previousTime).asMilliseconds());

		while(SDL_PollEvent(&sdlEvent) != 0)
		{
			switch(sdlEvent.type)
			{
			case SDL_EventType::SDL_QUIT:
				return 0;
			case SDL_EventType::SDL_KEYDOWN:
			{
				if(!gameCopy.handleKeyDown(sdlEvent.key.keysym.scancode))
				{
					return 0;
				}
			} break;
			case SDL_EventType::SDL_KEYUP:
			{
				gameCopy.handleKeyUp(sdlEvent.key.keysym.scancode);
			}
			}
		}
		
		gameCopy.update(deltaTime);
		game.store(gameCopy);

		previousTime = currentTime;

		SDL_Delay(10);

	} while (true);

	return 0;
}