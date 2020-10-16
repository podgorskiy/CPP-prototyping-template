#pragma once
#include "board.h"
#include "misc.h"


namespace chessis
{
	inline int Evaluate(const Board& board, Turn::Enum turn, int depth)
	{
		int distance_score = 0;
		for (int i = 0; i < board.w_ops_count; ++i)
		{
			for (int j = 0; j < board.b_ops_count; ++j)
			{
				if (board.black_ops[j].health != 0 && board.white_ops[i].health != 0)
				{
					float d = sqr(board.black_ops[j].x - board.white_ops[i].x) +
					          sqr(board.black_ops[j].y - board.white_ops[i].y);
					distance_score -= int(sqrtf(d) * (board.white_ops[i].get_cost() - board.black_ops[j].get_cost()) / 1000.0f);
				}
			}
		}

		int eog_penalty = 0;
		bool black_win = board.white_total_health == 0;
		bool white_win = board.black_total_health == 0;

		if (black_win) { eog_penalty = -1000 * 100 - depth * 10; }
		if (white_win) { eog_penalty = 1000 * 100 + depth * 10; }

		int e = board.white_total_health - board.black_total_health + distance_score + eog_penalty;

		++board.positions;
		return (turn == Turn::WhitePLay ? e : -e);
	}

	inline bool game_over(const Board& board)
	{
		return board.white_total_health == 0 || board.black_total_health == 0;
	}
}
