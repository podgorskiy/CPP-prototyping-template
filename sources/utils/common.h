#pragma once
#include "glm_printers.h"
#include <spdlog/spdlog.h>
#include <stdexcept>
#include <string>


namespace utils
{
	constexpr float k_mm_per_inch = 25.4;
	constexpr float k_pi = 3.14159265358979323846;
	constexpr float k_2pi = 2.0 * 3.14159265358979323846;
	constexpr float k_pi2 = 3.14159265358979323846 / 2.0;
	constexpr float k_3pi4 = 3.0 * 3.14159265358979323846 / 4.0;
	constexpr float k_e = 2.71828182845904523536;
	constexpr float k_ln2 = 0.693147180559945309417232121458;
	constexpr float k_sqrt2 = 1.41421356237309504880;


	std::string get_cwd();

	namespace details
	{
		struct vaholder
		{
		protected:
			va_list ap;
		};
	}

	class runtime_error: public details::vaholder, public std::runtime_error
	{
	public:
		explicit runtime_error(const std::string& fmt_str, ...);
		explicit runtime_error(const char* fmt_str, ...);
		runtime_error(const runtime_error &) = default;
		runtime_error(runtime_error &&) = default;
		~runtime_error() override = default;
	};

	struct Humanize
	{
		Humanize(uint64_t x): x(x){}
		operator uint64_t() { return x; }
		uint64_t x;
	};
}

template<>
struct fmt::formatter<utils::Humanize>
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const utils::Humanize& x, FormatContext& ctx)
	{
		const char* suf[] = { "", "k", "M", "G", "T", "P", "E"};
		int i = 0;
		uint64_t value = x.x;
		uint64_t decimal = 0;
		for (;  i < 7 && value > 10000; ++i)
		{
			decimal = value % 1024;
			decimal = 10 * (decimal + 512) / 1024;
			value /= 1024;
		}
		if (decimal == 10)
		{
			++value;
			decimal = 0;
		}

		int d = static_cast<int>(value);
		if (decimal == 0)
		{
			return fmt::format_to(ctx.out(), "{}{}",  d, suf[i]);
		}
		else
		{
			return fmt::format_to(ctx.out(), "{}.{}{}",  d, decimal, suf[i]);
		}
	}
};
