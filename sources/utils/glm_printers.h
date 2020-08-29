#pragma once
#include <glm/glm.hpp>
#include <spdlog/spdlog.h>
#include <spdlog/fmt/fmt.h>


template<typename T>
struct fmt::formatter<glm::vec<2, T> >
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const glm::vec<2, T>& vec, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({0}, {1})", vec.x, vec.y);
	}
};

template<typename T>
struct fmt::formatter<glm::vec<3, T> >
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const glm::vec<2, T>& vec, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({0}, {1}, {2})", vec.x, vec.y, vec.z);
	}
};

template<typename T>
struct fmt::formatter<glm::vec<4, T> >
{
	template<typename ParseContext>
	constexpr auto parse(ParseContext& ctx)
	{
		return ctx.begin();
	}

	template<typename FormatContext>
	auto format(const glm::vec<4, T>& vec, FormatContext& ctx)
	{
		return fmt::format_to(ctx.out(), "({0}, {1}, {2}, {3})", vec.x, vec.y, vec.z, vec.w);
	}
};
