#include <doctest.h>
#include <chessis/chessis.h>


TEST_CASE("[BASIC] moves")
{
	SUBCASE("Basic")
	{
		chessis::Board board;
		chessis::make_board(board, ".pp##...\n"
		                           "....#...\n"
		                           "..#.....\n"
		                           "......##\n"
		                           "##......\n"
		                           ".....#..\n"
		                           "...#....\n"
		                           "...##PP.\n");

		auto move1 = chessis::FindBestMove(board, 12, chessis::Turn::BlackPlay);

//		REQUIRE_THROWS({
//			Render::Uniform aa(-1, Render::VarType::BYTE, 1);
//			aa.ApplyValue("!");
//		});


	}
}
