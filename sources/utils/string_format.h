#pragma once
#include <string>
#include <stdarg.h>
#include <memory.h>

namespace utils
{
	template <typename I>
	inline char* int2hex(char* dst, I w, size_t hex_len = sizeof(I) << 1U)
	{
		static const char* digits = "0123456789ABCDEF";
		memset(dst, '0', hex_len);
		for (size_t i = 0, j = (hex_len - 1) * 4; i<hex_len; ++i, j -= 4)
			dst[i] = digits[(w >> j) & 0x0f];
		return &(*(dst + hex_len) = '\0');
	}

	template <typename I>
	inline std::string int2hex(I w, size_t hex_len = sizeof(I) << 1U)
	{
		std::string rc(hex_len, '0');
		int2hex(&rc[0], w, hex_len);
		return rc;
	}

	std::string format(const char* fmt_str, va_list ap);

	std::string format(const std::string& fmt_str, ...);

	std::string format(const char* fmt_str, ...);
}
