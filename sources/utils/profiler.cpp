#include "profiler.h"

#include <spdlog/spdlog.h>
#include <stdint.h>

ScopeProfiler::ScopeProfiler(const char* name): m_name(name), m_start(Clock::now()), m_quiet(false)
{}

std::chrono::nanoseconds ScopeProfiler::GetTime()
{
	m_quiet = true;
	return Clock::now() - m_start;
}

ScopeProfiler::~ScopeProfiler()
{
	if (m_quiet)
	{
		return;
	}
	const char* suf[] = { "ns", "us", "ms", "s"};
	int64_t difference = GetTime().count();
	int i = 0;
	for (;  i < 4 && difference > 10000; ++i)
	{
		difference /= 1000;
	}
	int d = static_cast<int>(difference);
	spdlog::info("{:>30}: {}{}", m_name, d, suf[i]);
}
