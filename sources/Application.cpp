#include "Application.h"
#include <chessis/chessis.h>
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <string.h>
#include <fsal.h>
#include <stdlib.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>
#include <time.h>
#define NONIUS_RUNNER
#include <nonius/nonius.h++>
#include <iterator>
#include <string>
#include "utils/doc_test_runner.h"


using namespace chessis;


Application::Application(int argc, const char* const* argv)
{
	static_assert(sizeof(Piece) == sizeof(uint32_t));

	run_doc_tests(argc, argv);

    nonius::configuration cfg;
    cfg.samples = 10;
    cfg.verbose = true;
    nonius::benchmark benchmarks[] = {
        nonius::benchmark("FindBestMove1", [](nonius::chronometer meter){
			chessis::Board board;
            chessis::make_board(board, ".pp##...\n"
                                       "....#...\n"
                                       "..#.....\n"
                                       "......##\n"
                                       "##......\n"
                                       ".....#..\n"
                                       "...#....\n"
									   "...##PP.\n");

            meter.measure([&](int i) {
				auto move1 = FindBestMove(board, 12, Turn::BlackPlay);
				DoMove(board, move1, Turn::BlackPlay);
				auto move2 = FindBestMove(board, 12, Turn::WhitePLay);
				DoMove(board, move2, Turn::WhitePLay);
            });
        }),
        nonius::benchmark("FindBestMove3", [](nonius::chronometer meter){
			chessis::Board board;
            chessis::make_board(board, ".p##...\n"
                                       "....#...\n"
									   "...##P.\n");

            meter.measure([&](int i) {
				auto move1 = FindBestMove(board, 18, Turn::BlackPlay);
				DoMove(board, move1, Turn::BlackPlay);
				auto move2 = FindBestMove(board, 18, Turn::WhitePLay);
				DoMove(board, move2, Turn::WhitePLay);
            });
        })
    };
    nonius::go(cfg, std::begin(benchmarks), std::end(benchmarks), nonius::standard_reporter());

    // exit(0);

	fsal::FileSystem fs;
	std::string b = fs.Open("../board.txt");
	chessis::make_board(board, b);
	srand(time(0));
}


Application::~Application()
{
}

void Application::Draw(float time)
{
	static Turn::Enum turn = Turn::WhitePLay;

	chessis::DebugUI(board, turn);

}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
