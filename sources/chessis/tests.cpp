#include <doctest.h>
#include <chessis/chessis.h>


TEST_CASE("[BASIC] moves")
{
	SUBCASE("Basic")
	{
		chessis::Board board;
		chessis::make_board(board, "####..\n"
		                           "#a....\n"
		                           "...A..\n"
		                           "......\n"
		                           "......\n");

		auto move1 = chessis::FindBestMove(board, 6, chessis::Turn::WhitePLay);
		CHECK_EQ(move1.action, chessis::Move::move);
		CHECK_EQ(move1.dir, chessis::Direction::Left);
		CHECK_EQ(move1.op_id, 0);
		CHECK_EQ(move1.new_eval, 1000 * 100 + board.white_total_health + 3);

		printf("a");

//		REQUIRE_THROWS({
//			Render::Uniform aa(-1, Render::VarType::BYTE, 1);
//			aa.ApplyValue("!");
//		});


	}
}





