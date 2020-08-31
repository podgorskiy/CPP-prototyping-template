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
				int enemy_opp_id = board.get_opp_id(x, y, turn == Turn::BlackPlay, turn == Turn::WhitePLay);
				assert(enemy_opp_id != -1);
				auto& enemy_opp = enemy_ops[enemy_opp_id];
				if (!final)
				{
					board.op_history.emplace_back(enemy_opp, enemy_opp_id);
				}

				enemy_opp.health -= 1;
				if (enemy_opp.health <= 0)
				{
					enemy_opp.health = 0;
					enemy_opp.type = Piece::Dead;
					board.clear_cell(enemy_opp.x, enemy_opp.y);
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
				auto& enemy_opp = enemy_ops[backup.second];
				enemy_opp = backup.first;
				board.set_cell_op(enemy_opp.x, enemy_opp.y, backup.second, Next(turn));
				board.op_history.pop_back();
				break;
			}
		}
	}
}
