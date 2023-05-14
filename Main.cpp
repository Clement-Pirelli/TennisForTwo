#include <SDL/SDL.h>
#include "SDLUtils.h"
#include "scope_exit.h"
#include "vec.h"
#include "Timer.h"
#include "GameState.h"


void audioCallback(void* userData, Uint8* stream, int len)
{
	GameState& state = *static_cast<GameState*>(userData);
	const Channels chans = state.channels.load();

	float* floatStream = reinterpret_cast<float*>(stream);
	const int floatLen = len / 4;

	unsigned long long currentTick = state.audioTick.fetch_add(1) % frequency;

	for(int i = 0; i < floatLen; i+=2)
	{
		const float sample = static_cast<float>(i) / static_cast<float>(floatLen);
		const float loopedTime = static_cast<float>(currentTick) / static_cast<float>(frequency);

		const vec2 drawn = [&] {
			switch(currentTick % 3)
			{
			case 0:
				return params::tennisFloor.draw(sample, loopedTime);
			case 1:
				return params::tennisNet.draw(sample, loopedTime);
			case 2:
				return state.ballPosition.load();
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

	GameState state;

	const auto [deviceId, spec] = utils::OpenAudioDevice(&audioCallback, &state);
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
		const Time currentTime = Time::now();
		const Time dt = currentTime - previousTime;

		while(SDL_PollEvent(&sdlEvent) != 0)
		{
			switch(sdlEvent.type)
			{
			case SDL_EventType::SDL_QUIT:
				return 0;
			case SDL_EventType::SDL_KEYDOWN:
			{
				if(!state.handleKeyDown(sdlEvent.key.keysym.scancode))
				{
					return 0;
				}
			} break;
			}
		}

		const float deltaTime = static_cast<float>(dt.asMilliseconds());
		
		state.updateBall(deltaTime);

		previousTime = currentTime;

		SDL_Delay(10);

	} while (true);

	return 0;
}