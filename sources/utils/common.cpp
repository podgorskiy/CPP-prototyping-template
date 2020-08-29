#include "common.h"
#include "string_format.h"

#include <stdarg.h>

#ifdef WIN32
#include <direct.h>
#define getcwd _getcwd
#else
#include <unistd.h>
#define getcwd getcwd
#endif


std::string utils::get_cwd()
{
	char buffer[FILENAME_MAX];
	getcwd(buffer, FILENAME_MAX);
	return buffer;
}

utils::runtime_error::runtime_error(const std::string& fmt_str, ...): runtime_error(fmt_str.c_str())
{}

utils::runtime_error::runtime_error(const char* fmt_str, ...): details::vaholder(),
		std::runtime_error(utils::format(fmt_str, (va_start(ap, fmt_str), ap)))
{
	va_end(ap);
}
