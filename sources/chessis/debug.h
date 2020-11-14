#pragma once
#include <imgui.h>
#include "board.h"
#include "eval.h"
#include "rules.h"
#include "negamax.h"
#include <vector>


namespace chessis
{
	inline Move InferCommand(const Board& board, int op_id, int x, int y)
	{
		const Piece* ops = board.turn == Turn::WhitePLay ? board.white_ops : board.black_ops;
		const Piece& op = ops[op_id];
		if (board.valid_coord(x, y))
		{
			if (board.get_cell(x, y) == Board::Walkable)
			{
				if (x == op.x - 1 && y == op.y)
				{ return Move(0, Move::move, Direction::Left, op_id); }
				else if (x == op.x + 1 && y == op.y)
				{ return Move(0, Move::move, Direction::Right, op_id); }
				else if (x == op.x && y == op.y - 1)
				{ return Move(0, Move::move, Direction::Up, op_id); }
				else if (x == op.x && y == op.y + 1)
				{ return Move(0, Move::move, Direction::Down, op_id); }
			}
			else
			{
				const Piece& enemy = board.get_opp(x, y, board.turn == Turn::BlackPlay, board.turn == Turn::WhitePLay);
				if (enemy.type != Piece::Dead)
				{
					if (x == op.x - 1 && y == op.y)
					{ return Move(0, Move::attack, Direction::Left, op_id); }
					else if (x == op.x + 1 && y == op.y)
					{ return Move(0, Move::attack, Direction::Right, op_id); }
					else if (x == op.x && y == op.y - 1)
					{ return Move(0, Move::attack, Direction::Up, op_id); }
					else if (x == op.x && y == op.y + 1)
					{ return Move(0, Move::attack, Direction::Down, op_id); }
				}
			}
		}
		return Move(0, Move::end, Direction::NoDir, -1);
	}

	inline void DebugUI(Board& board)
	{
		const char* labels[8] = {"", "Block", "WP", "BP", "WK", "BK", "WA", "BA"};
		const ImVec4 colors[8] = {
				ImVec4(0.255, 0.255, 0.255, 1.0),
				ImVec4(0.7, 0.8, 0.7, 1.0),
				ImVec4(0.4, 0.4, 0.9, 1.0),
				ImVec4(0.9, 0.4, 0.4, 1.0),
				ImVec4(0.4, 0.7, 0.9, 1.0),
				ImVec4(0.9, 0.7, 0.4, 1.0),
				ImVec4(0.4, 0.7, 0.9, 1.0),
				ImVec4(0.9, 0.7, 0.4, 1.0),
		};
		ImGui::Begin("Board");

		ImGui::Columns(2);

		static Move last_move(0);
		static std::vector<Move> moves;
		ImVec2 poses[10][20];

		int e = Evaluate(board, 0);

		bool do_move = false;
		char buff[256];
		for (int i = 0; i < board.size_y; ++i)
		{
			for (int j = 0; j < board.size_x; ++j)
			{
				auto pos = ImGui::GetCursorPos();
				poses[i][j] = pos;
				ImGui::PushID(i * board.size_x + j);
				int status = board.cell_state.data()[i * board.size_x + j];
				const chessis::Piece* p = &chessis::nop();
				if (status & Board::HasW)
				{
					int id = (status & 0xF00) >> 8;
					p = &board.white_ops[id];
					status = ((board.white_ops[id].type) << 1u) | 0u;
				}
				if (status & Board::HasB)
				{
					int id = (status & 0xF00) >> 8;
					p = &board.black_ops[id];
					status = ((board.black_ops[id].type) << 1u) | 1u;
				}
				ImGui::PushStyleColor(ImGuiCol_Button, colors[status]);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[status]);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[status]);
				ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
				sprintf(buff, "%s:%d", labels[status], p->health);
				if (p->health == 0)
					sprintf(buff, "%s", labels[status]);

				ImGui::Button(buff, ImVec2(60, 60));

				if (board.turn == Turn::WhitePLay)
				{
					int playable_op = board.get_opp_id(j, i, true, false);
					if (playable_op != -1 && ImGui::BeginDragDropSource())
					{
						ImGui::SetDragDropPayload("op", &playable_op, sizeof(int));
						ImGui::EndDragDropSource();
					}

					if (ImGui::BeginDragDropTarget())
					{
						auto* payload = ImGui::AcceptDragDropPayload("op", ImGuiDragDropFlags_AcceptPeekOnly);
						if (payload)
						{
							int op_id = *((int*) (payload->Data));
							Move cmd = InferCommand(board, op_id, j, i);
							if (cmd.action != Move::end)
							{
								if (ImGui::AcceptDragDropPayload("op"))
								{
									DoMove(board, cmd, false);
									moves.clear();
									do_move = true;
									last_move = cmd;
								}
							}
						}
						ImGui::EndDragDropTarget();
					}
				}

				ImGui::PopStyleColor(4);
				if (j + 1 != board.size_x)
				{ ImGui::SameLine(); }
				ImGui::PopID();
			}
		}

		for (int i = 0; i < board.size_y; ++i)
		{
			for (int j = 0; j < board.size_x; ++j)
			{
				auto pos2 = ImGui::GetCursorPos();
				for (auto& move: moves)
				{
					Piece* ops = board.turn == Turn::WhitePLay ? board.white_ops : board.black_ops;
					Piece& op = ops[move.op_id];

					int _x = op.x;
					int _y = op.y;
					MoveCoord(_x, _y, move.dir);
					if (_x == j && _y == i)
					{
						auto pos_copy = poses[i][j];
						pos_copy.x += 20;
						pos_copy.y += 20;
						int tmp_x = 0;
						int tmp_y = 0;
						MoveCoord(tmp_x, tmp_y, move.dir);
						pos_copy.x -= tmp_x * 18;
						pos_copy.y -= tmp_y * 18;
						ImGui::SetCursorPos(pos_copy);
						int diff = move.new_eval - e;
						ImGui::TextColored(ImVec4(diff > 0 ? 0.2 : 1.0, diff > 0 ? 1.0 : 0.2, 0.2, 255), "%d", diff);
						ImGui::SetCursorPos(pos2);
					}
				}
			}
		}

		for (int i = 0; i < board.size_y; ++i)
		{
			for (int j = 0; j < board.size_x; ++j)
			{
				if (last_move.action == Move::attack)
				{
					Piece* ops = board.turn != Turn::WhitePLay ? board.white_ops : board.black_ops;
					Piece& op = ops[last_move.op_id];

					int _x = op.x;
					int _y = op.y;
					if (_x == j && _y == i)
					{
						ImGui::SetCursorPos(poses[i][j]);
                        const ImVec2 p = ImGui::GetCursorScreenPos();

						auto pos_copy = p;
						pos_copy.x += 30;
						pos_copy.y += 30;
						auto pos_copy2 = pos_copy;
						int tmp_x = 0;
						int tmp_y = 0;
						MoveCoord(tmp_x, tmp_y, last_move.dir);
						pos_copy.x += tmp_x * 4;
						pos_copy.y += tmp_y * 4;
						pos_copy2.x += tmp_x * 70;
						pos_copy2.y += tmp_y * 70;
                        ImDrawList* draw_list = ImGui::GetWindowDrawList();
						draw_list->AddLine(pos_copy, pos_copy2, ImColor(ImVec4(1.0, 0.2, 0.2, 255)), 3.);

						//ImGui::SetCursorPos(pos_copy);
						//ImGui::TextColored(ImVec4(0.2, 1.0, 0.2, 255), "X");
						//ImGui::SetCursorPos(pos2);
					}
				}
			}
		}

		ImGui::NextColumn();

		int t = board.positions;
		ImGui::Text("Evaluation: %d", Evaluate(board, 0));
		board.positions = t;

		static int white_depth = 6;
		static int black_depth = 6;
		ImGui::InputInt("DepthBlack", &black_depth);
		ImGui::InputInt("DepthWhite", &white_depth);

		if (ImGui::Button("Skip"))
		{
			board.turn = Next(board.turn);
			do_move = true;
		}
		static bool autoplay = false;
		static bool play_opponent = true;
		static bool autoeval = false;
		ImGui::Checkbox("Autoplay", &autoplay);
		ImGui::Checkbox("Play opponent", &play_opponent);
		ImGui::Checkbox("Autoeval", &autoeval);
		if (ImGui::Button("Make move") || (board.turn == Turn::BlackPlay && do_move && play_opponent) || (autoplay && !game_over(board)))
		{
			board.positions = 0;
			//if (turn == BlackPlay)
			//	std::swap(alpha, beta);
			int depth = board.turn == Turn::BlackPlay ? black_depth : white_depth;

			last_move = FindBestMove(board, depth);
			moves.clear();

			DoMove(board, last_move);
			if (board.turn == Turn::WhitePLay && autoeval)
			{
			    depth = board.turn == Turn::BlackPlay ? black_depth : white_depth;
				moves = ReturnAllMoves<true>(board, depth);
			}
		}
//		if (ImGui::Button("Eval moves. No prunning"))
//		{
//			board.positions = 0;
//			moves = ReturnAllMoves<false>(board, depth, turn);
//		}

		if (ImGui::Button("Eval moves. Alpha beta prunning"))
		{
			board.positions = 0;
			int depth = board.turn == Turn::BlackPlay ? black_depth : white_depth;
			moves = ReturnAllMoves<true>(board, depth);
		}

		if (ImGui::Button("Undo move"))
		{
			UndoMove(board);
			board.turn = Next(board.turn);
			moves.clear();
		}

		ImGui::Text("Evaluations: %d", board.positions);

		switch (last_move.action)
		{
			case Move::move:
			{
				ImGui::Text("Enemy action: move");
				break;
			}
			case Move::attack:
			{
				ImGui::Text("Enemy action: attack");
				break;
			}
		}

		switch (last_move.dir)
		{
			case Direction::Left:
				ImGui::Text("Enemy dir: Left");
				break;
			case Direction::Right:
				ImGui::Text("Enemy dir: Right");
				break;
			case Direction::Down:
				ImGui::Text("Enemy dir: Down");
				break;
			case Direction::Up:
				ImGui::Text("Enemy dir: Up");
				break;
		}
		int player_health = 0;
		for (int i = 0; i < board.w_ops_count; ++i)
		{
			player_health += board.white_ops[i].get_cost();
		}
		int enemy_health = 0;
		for (int i = 0; i < board.b_ops_count; ++i)
		{
			enemy_health += board.black_ops[i].get_cost();
		}
		ImGui::Text("Health: %d : %d", player_health, enemy_health);

		if (ImGui::Button("Print board"))
		{
			board.dump_board();
		}
		ImGui::End();
	}
}
