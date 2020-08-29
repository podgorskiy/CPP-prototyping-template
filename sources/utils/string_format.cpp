#include "string_format.h"

#include <stdarg.h>
#include <string.h>
#include <memory>


std::string utils::format(const char* fmt_str, va_list ap)
{
	int final_n, n = ((int) strlen(fmt_str)) * 2;
	std::unique_ptr<char[]> formatted;
	while (true)
	{
		va_list ap_copy;
		va_copy(ap_copy, ap);
		formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
		strcpy(&formatted[0], fmt_str);
		final_n = vsnprintf(&formatted[0], n, fmt_str, ap_copy);
		if (final_n < 0 || final_n >= n)
		{
			n += abs(final_n - n + 1);
		}
		else
		{
			break;
		}
	}
	return std::string(formatted.get());
}

std::string utils::format(const char* fmt_str, ...)
{
	va_list ap;
	va_start(ap, fmt_str);
	std::string result = format(fmt_str, ap);
	va_end(ap);
	return result;
}

std::string utils::format(const std::string& fmt_str, ...)
{
	return format(fmt_str.c_str());
}
