#pragma once
#include <SDL/SDL.h>

constexpr unsigned int frequency = 192000;
constexpr unsigned int samples = 1024;

#define ENSURE(x, successMessage, errMessage) \
	if(!(x)) \
	{ \
		SDL_Log(errMessage ". SDL Error: %s", SDL_GetError()); \
		return 1; \
	} \
	else \
	{ \
		SDL_Log(successMessage "\n"); \
	} 

namespace utils
{
	struct AudioDevice
	{
		SDL_AudioDeviceID id;
		SDL_AudioSpec spec;
	};

	AudioDevice OpenAudioDevice(void (*audioCallback)(void*, Uint8*, int))
	{
		SDL_AudioSpec audioSpec{};

		const SDL_AudioSpec want
		{
			.freq = frequency,
			.format = AUDIO_F32,
			.channels = 2,
			.samples = samples,
			.callback = audioCallback
		};

		const SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(nullptr, 0, &want, &audioSpec, 0);

		return { deviceId, audioSpec };
	}
}
