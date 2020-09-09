#define DOCTEST_CONFIG_IMPLEMENT

#include <doctest.h>

#undef WARN


void run_doc_tests(int argc, const char* const* argv)
{
#ifndef __EMSCRIPTEN__
	doctest::Context context;
	context.setOption("rand-seed", 1);
	context.setOption("order-by", "file");
	context.applyCommandLine(argc, argv);
	context.setOption("no-breaks", true); // don't break in the debugger when assertions fail
	int res = context.run(); // run queries, or run tests unless --no-run is specified
	if (context.shouldExit()) // important - query flags (and --exit) rely on the user doing this
	{
		exit(res);
	}
	context.clearFilters(); // removes all filters added up to this point

#endif
}
