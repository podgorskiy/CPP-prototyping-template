#pragma once
#include "board.h"


namespace chessis
{
	void GenerateMoves(const Board& board, Turn::Enum turn, stack_buffer<Move, MAX_SUCCESSORS>& successors)
	{
		const Piece* ops = turn == Turn::WhitePLay ? board.white_ops : board.black_ops;
		int count = turn == Turn::WhitePLay ? board.w_ops_count : board.b_ops_count;

		for (int i = 0; i < count; ++i)
		{
			const Piece& op = ops[i];

			if (op.type == Piece::Dead)
			{
				continue;
			}

			// attack
			for (int _dir = 0; _dir < Direction::End; ++_dir)
			{
				auto dir = (Direction::Enum) _dir;
				int x = op.x;
				int y = op.y;
				MoveCoord(x, y, dir);
				auto& enemy_opp = board.get_opp(x, y, turn == Turn::BlackPlay, turn == Turn::WhitePLay);

				if (enemy_opp.type != 0)
				{
					successors.emplace_back(INT_MIN, Move::attack, dir, i);
				}
			}

			// move
			for (int _dir = 0; _dir < Direction::End; ++_dir)
			{
				auto dir = (Direction::Enum) _dir;

				if (board.get_cell(op.x, op.y, dir) == Board::Walkable)
				{
					successors.emplace_back(INT_MIN, Move::move, dir, i);
				}
			}
		}
	}


	void DoMove(Board& board, Move cmd, Turn::Enum turn, bool final = false)
	{
		Piece* ops = turn == Turn::WhitePLay ? board.white_ops : board.black_ops;
		Piece* enemy_ops = turn == Turn::BlackPlay ? board.white_ops : board.black_ops;
		int& total_health = turn == Turn::BlackPlay ? board.white_total_health : board.black_total_health;
		Piece& op = ops[cmd.op_id];
		if (!final)
		{
			board.cmd_history.emplace_back(cmd, turn);
		}
		switch (cmd.action)
		{
			case Move::move:
			{
				board.move_piece(op.x, op.y, cmd.dir, turn);
				break;
			}
			case Move::attack:
			{
				int x = op.x;
				int y = op.y;
				MoveCoord(x, y, cmd.dir);
				int enemy_opp_id = board.get_opp_id_nch(x, y, turn == Turn::BlackPlay, turn == Turn::WhitePLay);
				assert(enemy_opp_id != -1);
				auto& enemy_opp = enemy_ops[enemy_opp_id];
				Piece enemy_opp_backup = enemy_opp;

				int dmg = 1;
				int cost_before = enemy_opp.get_cost();
				enemy_opp.health -= dmg;

				if (enemy_opp.health <= 0)
				{
					enemy_opp.health = 0;
					enemy_opp.type = Piece::Dead;
					board.clear_cell_nch(enemy_opp.x, enemy_opp.y);
				}
				int cost_after = enemy_opp.get_cost();
				int total_health_diff = cost_before - cost_after;

				total_health -= total_health_diff;

				if (!final)
				{
					board.op_history.emplace_back(enemy_opp_backup, enemy_opp_id, total_health_diff);
				}
				break;
			}
		}
	}

	void UndoMove(Board& board)
	{
		if (board.cmd_history.empty())
		{
			return;
		}
		auto& _cmd = board.cmd_history.back();
		Move cmd = _cmd.first;
		Turn::Enum turn = _cmd.second;
		board.cmd_history.pop_back();

		Piece* ops = turn == Turn::WhitePLay ? board.white_ops : board.black_ops;
		Piece* enemy_ops = turn == Turn::BlackPlay ? board.white_ops : board.black_ops;
		int& total_health = turn == Turn::BlackPlay ? board.white_total_health : board.black_total_health;
		Piece& op = ops[cmd.op_id];

		switch (cmd.action)
		{
			case Move::move:
			{
				board.move_piece(op.x, op.y, GetOpposit(cmd.dir), turn);
				break;
			}
			case Move::attack:
			{
				auto& backup = board.op_history.back();
				auto& enemy_opp = enemy_ops[std::get<1>(backup)];
				enemy_opp = std::get<0>(backup);
				board.set_cell_op_nch(enemy_opp.x, enemy_opp.y, std::get<1>(backup), Next(turn));
				board.op_history.pop_back();
				total_health += std::get<2>(backup);
				break;
			}
		}
	}
}
