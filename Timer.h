#pragma once

class Time
{
public:
	[[nodiscard]]
	static Time now();

	[[nodiscard]]
	unsigned long long asMilliseconds() const;
	[[nodiscard]]
	unsigned long long asSeconds() const;

	[[nodiscard]]
	Time operator-(const Time& other) const;
	[[nodiscard]]
	Time operator+(const Time& other) const;

	void operator-=(const Time& other);
	void operator+=(const Time& other);

private:

	constexpr Time(unsigned long long givenTicks) : ticks(givenTicks) {}

	unsigned long long ticks;
};