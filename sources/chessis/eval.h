#pragma once
#include "board.h"
#include "misc.h"


namespace chessis
{
	int Evaluate(const Board& board, Turn::Enum turn, int depth)
	{
		int distance = 0;
//		for (int i = 0; i < board.w_ops_count; ++i)
//		{
//			for (int j = 0; j < board.b_ops_count; ++j)
//			{
//				if (board.black_ops[j].health != 0 && board.white_ops[i].health != 0)
//				{
//					float d = sqr(board.black_ops[j].x - board.white_ops[i].x) +
//					          sqr(board.black_ops[j].y - board.white_ops[i].y);
//					distance += d;// * (for_black ? (gs.white_ops[i].health / gs.black_ops[j].health + 1) : (gs.black_ops[j].health / gs.white_ops[i].health + 1));
//				}
//			}
//		}

		int e = board.white_total_health - board.black_total_health + distance + depth;
		if (board.white_total_health == 0) { e -= 1000 * 100; }
		if (board.black_total_health == 0) { e += 1000 * 100; }

		++board.positions;
		return (turn == Turn::WhitePLay ? e : -e);
	}

	bool game_over(const Board& board)
	{
		return board.white_total_health == 0 || board.black_total_health == 0;
	}
}
