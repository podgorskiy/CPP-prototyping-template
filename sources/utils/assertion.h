#pragma once
#include <spdlog/spdlog.h>
#include "utils/string_format.h"

#include <stdio.h>
#include <cstdlib>
#ifdef _WIN32
#define BREAKPOINT() { __debugbreak(); }
#elif __linux__
#include <signal.h>
#define BREAKPOINT() { raise(SIGTRAP); }
#else
#define BREAKPOINT() { __builtin_trap(); }
#endif

namespace Assert
{
	enum result
	{
		result_break,
		result_ignore_always,
		result_ignore_once,
	};

	result message(const char *file, int line, const char *condition, const char *fmt, ...);
}

#define ASSERT(X, ...) \
{\
	static bool _i=false;\
	if (!_i&&!(X))\
	{\
		spdlog::error("{:*^80}", "Assert!");\
		spdlog::error("{}(line: {}): {}",__FILE__,__LINE__,#X);\
		spdlog::error(utils::format(__VA_ARGS__));\
		Assert::result r=Assert::message(__FILE__,__LINE__,#X,__VA_ARGS__);\
		if(r==Assert::result_ignore_always)\
		{\
			_i=true;\
		}\
		else if(r==Assert::result_break)\
		{\
			BREAKPOINT();\
		}\
		else if(r==Assert::result_ignore_once)\
		{\
		}\
	}\
}

#define WARN(...)\
{\
	static bool _i=false;\
	if(!_i)\
	{\
		spdlog::warn("{:*^80}", "Warning!");\
		spdlog::warn("{}(line: {})",__FILE__,__LINE__);\
		spdlog::warn(__VA_ARGS__);\
		Assert::result r=Assert::message(__FILE__,__LINE__,nullptr,__VA_ARGS__);\
		if(r==Assert::result_ignore_always)\
		{\
			_i=true;\
		}\
		else if(r==Assert::result_break)\
		{\
			BREAKPOINT();\
		}\
		else if(r==Assert::result_ignore_once)\
		{\
		}\
	}\
}
