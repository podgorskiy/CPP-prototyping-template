#include <doctest.h>
#include <chessis/chessis.h>


TEST_CASE("[BASIC] moves")
{
	SUBCASE("Obvious best move")
	{
		chessis::Board board;
		chessis::make_board(board, "####..\n"
		                           "#a....\n"
		                           "...A..\n"
		                           "......\n"
		                           "......\n");

		auto move1 = chessis::FindBestMove(board, 6);
		CHECK_EQ(move1.action, chessis::Move::move);
		CHECK_EQ(move1.dir, chessis::Direction::Left);
		CHECK_EQ(move1.op_id, 0);
		CHECK_EQ(move1.new_eval, 1000 * 100 + board.white_total_health + 3);
	}
	SUBCASE("Random moves")
	{
		chessis::Board board;
		board.turn = chessis::Turn::WhitePLay;
		chessis::make_board(board, "####..\n"
		                           "#a...A\n"
		                           "...#..\n"
		                           "..#...\n"
		                           "a....A\n");
		chessis::Board board_copy = board;
		int count = 0;
		while (true)
		{
			auto move = chessis::RandomMove(board);
			if (move.action == chessis::Move::end)
				break;
			chessis::DoMove(board, move); count++;
		}
		printf("%d\n", count);
		for (int i = 0; i < count; ++i)
		{
			chessis::UndoMove(board);
		}
		CHECK_EQ(board.cell_state, board_copy.cell_state);
		CHECK_EQ(board.white_total_health, board_copy.white_total_health);
		CHECK_EQ(board.black_total_health, board_copy.black_total_health);
		for (int i = 0; i < board.b_ops_count; ++i)
			CHECK_EQ(board.black_ops[i], board_copy.black_ops[i]);
		for (int i = 0; i < board.w_ops_count; ++i)
			CHECK_EQ(board.white_ops[i], board_copy.white_ops[i]);

		printf("a");

//		REQUIRE_THROWS({
//			Render::Uniform aa(-1, Render::VarType::BYTE, 1);
//			aa.ApplyValue("!");
//		});


	}
}





