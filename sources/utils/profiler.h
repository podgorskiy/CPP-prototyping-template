#pragma once
#include <chrono>

#ifndef NO_PROFILING
#define Profile(X) ScopeProfiler scopeVar_##__LINE__(#X)
#else
#define Profile(X)
#endif

class ScopeProfiler
{
public:
	typedef std::chrono::high_resolution_clock Clock;

	explicit ScopeProfiler(const char* name);

	ScopeProfiler(const ScopeProfiler& other) = delete;
    ScopeProfiler& operator=(const ScopeProfiler&) = delete;

	std::chrono::nanoseconds GetTime();

	~ScopeProfiler();

private:
	const char* m_name;
	Clock::time_point m_start;
	bool m_quiet;
};
