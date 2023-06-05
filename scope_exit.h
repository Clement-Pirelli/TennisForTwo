#pragma once
#include <utility>

namespace details
{
	struct scope_exit_t {};
}

template<typename Callable_t>
struct scope_exit 
{
	scope_exit() = delete;
	scope_exit(const Callable_t& t) : callable(t) {}
	scope_exit(Callable_t&& t) : callable(std::move(t)) {}
	~scope_exit() { callable(); }

	Callable_t callable;
};

template<typename T>
auto operator+(details::scope_exit_t, T&& t)
{
	return scope_exit(std::forward<T>(t));
}

#define SCOPE_EXIT_CONCAT2(a, b, c) a ## b ## c
#define SCOPE_EXIT_CONCAT(a, b, c) SCOPE_EXIT_CONCAT2(a, b, c)
#define DEFER auto SCOPE_EXIT_CONCAT(scopeobject, __COUNTER__, __LINE__) = ::details::scope_exit_t{} + [&]()