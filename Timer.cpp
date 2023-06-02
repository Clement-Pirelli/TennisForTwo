#include "Timer.h"
#include <SDL/SDL.h>

Time Time::now()
{
	return SDL_GetTicks64();
}

unsigned long long Time::asMilliseconds() const
{
	return ticks;
}

unsigned long long Time::asSeconds() const
{
	return ticks / 1000u;
}

Time Time::operator-(const Time& other) const
{
	return { ticks - other.ticks };
}

Time Time::operator+(const Time& other) const
{
	return { ticks + other.ticks };
}

void Time::operator-=(const Time& other)
{
	ticks -= other.ticks;
}

void Time::operator+=(const Time& other)
{
	ticks += other.ticks;
}