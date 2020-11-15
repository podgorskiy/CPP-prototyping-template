#pragma once
#include "board.h"
#include "misc.h"


namespace chessis
{
	inline int Evaluate(const Board& board)
	{
		bool black_win = board.white_total_health == 0;
		bool white_win = board.black_total_health == 0;

		if (black_win) { return (board.turn == Turn::WhitePLay ? -1 : 1) * 1000 * 100; }
		if (white_win) { return (board.turn == Turn::WhitePLay ? 1 : -1) * 1000 * 100; }

		int distance_score = 0;
		float influence_on_w[MAX_OPS];
		float influence_on_b[MAX_OPS];
		for (int i = 0; i < board.w_ops_count; ++i)
		{
			influence_on_w[i] = 0.0f;
		}
		for (int i = 0; i < board.b_ops_count; ++i)
		{
			influence_on_b[i] = 0.0f;
		}
		for (int i = 0; i < board.w_ops_count; ++i)
		{
			for (int j = 0; j < board.b_ops_count; ++j)
			{
				if (board.black_ops[j].health != 0 && board.white_ops[i].health != 0)
				{
					float d = sqr(board.black_ops[j].x - board.white_ops[i].x) +
					          sqr(board.black_ops[j].y - board.white_ops[i].y);
					influence_on_w[i] += board.black_ops[j].get_cost() / d;
					influence_on_b[j] += board.white_ops[i].get_cost() / d;
					distance_score -= int(sqrtf(d) * (board.white_ops[i].get_cost() - board.black_ops[j].get_cost()) / 100.0f);
				}
			}
		}
		float influence_on_ws = 0;
		float influence_on_bs = 0;
		for (int i = 0; i < board.w_ops_count; ++i)
		{
			influence_on_ws = std::max(influence_on_w[i], influence_on_ws);
		}
		for (int i = 0; i < board.b_ops_count; ++i)
		{
			influence_on_bs = std::max(influence_on_w[i], influence_on_bs);
		}
		distance_score += (influence_on_bs - influence_on_ws) / 10;

		int e = board.white_total_health * 10 - board.black_total_health * 10 + distance_score;

		++board.positions;
		return (board.turn == Turn::WhitePLay ? e : -e);
	}

	inline int EvaluateClean(const Board& board)
	{
		int e = board.white_total_health - board.black_total_health;
		return (board.turn == Turn::WhitePLay ? e : -e);
	}

	inline bool game_over(const Board& board)
	{
		return board.white_total_health == 0 || board.black_total_health == 0;
	}
}
