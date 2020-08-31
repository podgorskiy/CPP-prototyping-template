#include "Application.h"
#include "utils/buffer.h"
#include "utils/stack_buffer.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <string.h>
#include <fsal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>

int move_number = 0;

#pragma pack(push,1)
struct Operator
{

	enum Type : uint8_t
	{
		Dead = 0x00,
		Pawn = 0x01,
		Knight = 0x02,
		Rook = 0x03,
	};

	Type type : 8;
	int8_t health : 8;
	uint8_t attribute : 6;
	uint8_t x : 4;
	uint8_t y : 4;
	uint8_t o : 2;

	int get_cost() const
	{
		switch (type)
		{
			case Operator::Pawn: return 100 * health;
			case Operator::Knight: return 200 * health;
			case Operator::Rook: return 300 * health;
		}
		return 0;
	}
};
#pragma pack(pop)

Operator noop({Operator::Dead, 0, 0, 0, 0, 0});


enum Turn
{
	WhitePLay,
	BlackPlay
};

Turn Next(Turn t)
{
	switch(t)
	{
		case WhitePLay: return BlackPlay;
		case BlackPlay: return WhitePLay;
	}
	return (Turn)-1;
}


struct GameState
{
	Operator white_ops[12];
	Operator black_ops[12];
};

struct Board
{
	enum state
	{
		Walkable = 0,
		Block = 0x1,
		HasW = 0x1000,
		HasB = 0x2000,
	};
	enum Direction
	{
		Left,
		Right,
		Down,
		Up,
		End,
		NoDir = End
	};

	static Direction GetOpposit(Direction dir)
	{
		switch(dir)
		{
			case Left: return Right;
			case Right: return Left;
			case Down: return Up;
			case Up: return Down;
			default:
				return NoDir;
		}
	}

	static void MoveCoord(int& x, int& y, Direction dir)
	{
		switch(dir)
		{
			case Left: --x; break;
			case Right: ++x; break;
			case Down: ++y; break;
			case Up: --y; break;
			default:
				return;
		}
	}

	glm::ivec2 size;
	buffer<int> cell_state;

	bool valid_coord(int x, int y) const { return !(x < 0 || y < 0 || x >= size.x || y >= size.y); }
	const int get_cell(int x, int y) const { return valid_coord(x, y) ? cell_state[x + y * size.x] : Block; }

	const int get_cell(int x, int y, Direction dir) const
	{
		MoveCoord(x, y, dir);
		return valid_coord(x, y) ? cell_state[x + y * size.x] : Block;
	}

	void set_cell(int x, int y, int n) { if (valid_coord(x, y)) cell_state[x + y * size.x] = n; }
	void set_cell_and_or(int x, int y, int a, int o) { if (valid_coord(x, y)) { auto& c = cell_state[x + y * size.x]; c &= a; c |= o; }}
	void clear_cell(int x, int y) { set_cell_and_or(x, y, 0xFF, 0); }
	void set_cell_op(int x, int y, int op_id, Turn turn) { set_cell_and_or(x, y, 0xFF, (op_id << 8) | (turn == WhitePLay ? Board::HasW:Board::HasB)); }

	void move_piece(int x, int y, Direction dir, Turn turn)
	{
		int piece = get_opp_id(gs, x, y, turn == WhitePLay, turn == BlackPlay);
		assert(piece != -1);
		clear_cell(x, y);
		MoveCoord(x, y, dir);
		Operator* ops = turn == WhitePLay ? gs.white_ops  : gs.black_ops;
		ops[piece].x = x;
		ops[piece].y = y;
		set_cell_op(x, y, piece, turn);
	}

	Operator& get_opp(GameState& gs, int x, int y, bool white, bool black) const
	{
		if (valid_coord(x, y))
		{
			auto c = get_cell(x, y);
			if (c & Board::HasW && white)
			{
				int id = (c & 0xF00) >> 8;
				return gs.white_ops[id];
			}
			else if (c & Board::HasB && black)
			{
				int id = (c & 0xF00) >> 8;
				return gs.black_ops[id];
			}
		}
		return noop;
	}

	int get_opp_id(GameState& gs, int x, int y, bool white, bool black) const
	{
		if (valid_coord(x, y))
		{
			auto c = get_cell(x, y);
			if ((c & Board::HasW) && white)
			{
				return  (c & 0xF00) >> 8;
			}
			else if ((c & Board::HasB) && black)
			{
				return (c & 0xF00) >> 8;
			}
		}
		return -1;
	}

	GameState gs;

	uint8_t b_ops_count = 0;
	uint8_t w_ops_count = 0;
};

const char* labels[6] = { "", "Block", "WP", "BP", "WK", "BK" };
const ImVec4 colors[6] = {
		ImVec4(0.255, 0.255, 0.255, 1.0),
		ImVec4(0.7, 0.8, 0.7, 1.0),
		ImVec4(0.4, 0.4, 0.9, 1.0),
		ImVec4(0.9, 0.4, 0.4, 1.0),
		ImVec4(0.4, 0.7, 0.9, 1.0),
		ImVec4(0.9, 0.7, 0.4, 1.0),
};



Board board;


Board::state get_cell_type(char c)
{
	switch(c)
	{
		case ' ':
		case '.':
			return Board::Walkable;
		case 'P':
		case 'K':
		case 'p':
		case 'k':
			return Board::Walkable;
		case '#':
			return Board::Block;
	}
	return Board::Block;
}

int sqr(int x) {return x * x;}

int Evaluate(const Board& board, const GameState& gs, Turn turn)
{
	int ew = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		ew += gs.white_ops[i].get_cost();
	}
	int eb = 0;
	for (int i = 0; i < board.b_ops_count; ++i)
	{
		eb -= gs.black_ops[i].get_cost();
	}
	int distance = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		for (int j = 0; j < board.b_ops_count; ++j)
		{
			if (gs.black_ops[j].health != 0 && gs.white_ops[i].health != 0)
			{
				float d = sqr(gs.black_ops[j].x - gs.white_ops[i].x) + sqr(gs.black_ops[j].y - gs.white_ops[i].y);
				distance += d;// * (for_black ? (gs.white_ops[i].health / gs.black_ops[j].health + 1) : (gs.black_ops[j].health / gs.white_ops[i].health + 1));
			}
		}
	}

	if (ew == 0) ew = -1000000;
	if (eb == 0) eb = 1000000;
	int e = ew + eb + distance;
	e += move_number;
	return (turn == WhitePLay ? e : -e);

	return -distance;
}

struct Command
{
	enum Action
	{
		move,
		attack,
		end
	};
	int new_eval = 0;
	Action action = end;
	Board::Direction dir = Board::NoDir;
	int op_id = -1;

	Command(): new_eval(INT_MIN), action(end), op_id(-1) {}
	Command(int new_eval): new_eval(new_eval), action(end), op_id(-1) {}
	Command(int new_eval, Action action, Board::Direction dir, int op_id): new_eval(new_eval), action(action), dir(dir), op_id(op_id) {}

	int operator ()() const { return new_eval; }
};


template<typename C>
typename C::value_type maximum(const C& v)
{
	typename C::size_type imax = 0;
	auto max = v[0]();
	for(typename C::size_type i = 0, l = v.size(); i < l; ++i)
	{
		auto c = v[i]();
		if (c > max)
		{
			imax = i;
			max = c;
		}
	}
	return v[imax];
}

template<typename C>
typename C::size_type argmaximum(const C& v)
{
	typename C::size_type imax = 0;
	auto max = v[0]();
	for(typename C::size_type i = 0, l = v.size(); i < l; ++i)
	{
		auto c = v[i]();
		if (c > max)
		{
			imax = i;
			max = c;
		}
	}
	return imax;
}


Command TraverseState(Board& board, GameState& gs, Turn turn, int depth)
{
	if (depth == 0)
	{
		return Command(Evaluate(board, gs, turn));
	}
	Operator* ops = turn == WhitePLay ? gs.white_ops  : gs.black_ops;
	int count = turn == WhitePLay ? board.w_ops_count  : board.b_ops_count;

	stack_buffer<Command, 12 * 4> successors;

	for (int i = 0; i < count; ++i)
	{
		Operator& op = ops[i];

		if (op.type == Operator::Dead)
			continue;

		// attack
		for (int _dir = 0; _dir < Board::End; ++_dir)
		{
			Board::Direction dir = (Board::Direction)_dir;
			int x = op.x;
			int y = op.y;
			Board::MoveCoord(x, y, dir);
			auto& enemy_opp = board.get_opp(gs, x, y, turn == BlackPlay, turn == WhitePLay);

			if (enemy_opp.type != 0)
			{
				Operator backup = enemy_opp;
				int id = (board.get_cell(backup.x, backup.y) & 0xF00u) >> 8u;

				enemy_opp.health -= 1;
				if (enemy_opp.health <= 0)
				{
					enemy_opp.health = 0;
					enemy_opp.type = Operator::Dead;
					board.clear_cell(enemy_opp.x, enemy_opp.y);
				}

				int eval = -TraverseState(board, gs, Next(turn), depth-1).new_eval;
				successors.emplace_back(eval, Command::attack, dir, i);
				enemy_opp = backup;
				board.set_cell_op(enemy_opp.x, enemy_opp.y, id, Next(turn));
			}
		}

		// move
		for (int _dir = 0; _dir < Board::End; ++_dir)
		{
			Board::Direction dir = (Board::Direction)_dir;

			if (board.get_cell(op.x, op.y, dir) == Board::Walkable)
			{
				board.move_piece(op.x, op.y, dir, turn);
				int eval = -TraverseState(board, gs, Next(turn), depth-1).new_eval;
				successors.emplace_back(eval, Command::move, dir, i);
				board.move_piece(op.x, op.y, Board::GetOpposit(dir), turn);
			}
		}
	}

	if (successors.empty())
	{
		return Command(Evaluate(board, gs, turn));
	}

	return maximum(successors);
}

Application::Application()
{
	static_assert(sizeof(Operator) == sizeof(uint32_t));

	fsal::FileSystem fs;
	std::string b = fs.Open("../board.txt");
	const char* ptr = b.c_str();
	int width = strstr(ptr, "\n") - ptr;
	int count = 0;
	const char *tmp = ptr;
	while(tmp = strstr(tmp, "\n"))
	{
	   count++;
	   tmp++;
	}

    int height = count;

	board.size = glm::ivec2(width, height);
	board.cell_state.resize(board.size.x * board.size.y);
	memset(board.cell_state.data(), 0, board.size.x * board.size.y * sizeof(int));

	for (int i = 0; i < board.size.y; ++i)
	{
		for (int j = 0; j < board.size.x; ++j)
		{
			board.cell_state.data()[i * board.size.x + j] = get_cell_type(ptr[i * (width + 1) + j]);
			switch(ptr[i * (width + 1) + j])
			{
					case 'p': board.gs.black_ops[board.b_ops_count] = Operator({Operator::Pawn, 2, 0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasB | (board.b_ops_count++ << 8);
						break;
					case 'P': board.gs.white_ops[board.w_ops_count] = Operator({Operator::Pawn, 2,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasW | (board.w_ops_count++ << 8);
						break;
					case 'k': board.gs.black_ops[board.b_ops_count] = Operator({Operator::Knight, 5,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasB | (board.b_ops_count++ << 8);
						break;
					case 'K': board.gs.white_ops[board.w_ops_count] = Operator({Operator::Knight, 5,  0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasW | (board.w_ops_count++ << 8);
						break;
			}
		}
	}
}


Application::~Application()
{
}

void MakeMove(Command tr, Turn turn)
{
	Operator* ops = turn == WhitePLay ? board.gs.white_ops : board.gs.black_ops;
	Operator& op = ops[tr.op_id];
	Operator* f = nullptr;
	switch (tr.action)
	{
		case Command::move:
		{
			board.move_piece(op.x, op.y, tr.dir, turn);
		}
		break;
		case Command::attack:
		{
			int x = op.x;
			int y = op.y;
			Board::MoveCoord(x, y, tr.dir);
			f = &board.get_opp(board.gs, x, y, true, true);
			f->health--;
		}
		break;
	}
	if (f && f->health <= 0)
	{
		f->type = Operator::Dead;
		board.clear_cell(f->x, f->y);
	}
	move_number++;
}

Command InferCommand(int op_id, Turn turn, int x, int y)
{
	Operator* ops = turn == WhitePLay ? board.gs.white_ops : board.gs.black_ops;
	Operator& op = ops[op_id];
	if (board.valid_coord(x, y))
	{
		if (board.get_cell(x, y) == Board::Walkable)
		{
			if (x == op.x - 1 && y == op.y) return Command(0, Command::move, Board::Left, op_id);
			else if (x == op.x + 1 && y == op.y) return Command(0, Command::move, Board::Right, op_id);
			else if (x == op.x && y == op.y - 1) return Command(0, Command::move, Board::Up, op_id);
			else if (x == op.x && y == op.y + 1) return Command(0, Command::move, Board::Down, op_id);
		}
		else
		{
			const Operator& enemy = board.get_opp(board.gs, x, y, turn == BlackPlay, turn == WhitePLay);
			if (enemy.type != Operator::Dead)
			{
				if (x == op.x - 1 && y == op.y) return Command(0, Command::attack, Board::Left, op_id);
				else if (x == op.x + 1 && y == op.y) return Command(0, Command::attack, Board::Right, op_id);
				else if (x == op.x && y == op.y - 1) return Command(0, Command::attack, Board::Up, op_id);
				else if (x == op.x && y == op.y + 1) return Command(0, Command::attack, Board::Down, op_id);
			}
		}
	}
	return Command(0, Command::end, Board::NoDir,  -1);
}

void Application::Draw(float time)
{
	static Turn turn = WhitePLay;

	ImGui::Begin("Board");

	ImGui::Columns(2);

	static Command enemy_cmd(0);

	bool do_move = false;

	for (int i = 0; i < board.size.y; ++i)
	{
		for (int j = 0; j < board.size.x; ++j)
		{
			ImGui::PushID(i * board.size.x + j);
			int status = board.cell_state.data()[i * board.size.x + j];
			if (status & Board::HasW)
			{
				int id = (status & 0xF00) >> 8;
				status = ((board.gs.white_ops[id].type) << 1u) | 0u;
			}
			if (status & Board::HasB)
			{
				int id = (status & 0xF00) >> 8;
				status = ((board.gs.black_ops[id].type) << 1u) | 1u;
			}
			ImGui::PushStyleColor(ImGuiCol_Button, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, colors[status]);
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.0, 0.0, 0.0, 1.0));
			ImGui::Button(labels[status], ImVec2(60, 60));

			if (turn == WhitePLay)
			{
				int playable_op = board.get_opp_id(board.gs, j, i, true, false);
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
						Command cmd = InferCommand(op_id, turn, j, i);
						if (cmd.action != Command::end)
						{
							if (ImGui::AcceptDragDropPayload("op"))
							{
								MakeMove(cmd, turn);
								turn = Next(turn);
								do_move = true;
							}
						}
					}
					ImGui::EndDragDropTarget();
				}
			}

			ImGui::PopStyleColor(4);
			if (j + 1 != board.size.x) ImGui::SameLine();
			ImGui::PopID();
		}
	}

	ImGui::NextColumn();

	ImGui::Text("Evaluation: %d", Evaluate(board, board.gs, WhitePLay));

	static int depth = 6;
	ImGui::InputInt("Depth", &depth);

	if (ImGui::Button("Skip"))
	{
		turn = Next(turn);
		do_move = true;
	}
	if (ImGui::Button("Make move") || (turn == BlackPlay && do_move))
	{
		enemy_cmd = TraverseState(board, board.gs, turn, depth);
		MakeMove(enemy_cmd, turn);
		turn = Next(turn);
		do_move = false;
	}

	switch (enemy_cmd.action)
	{
		case Command::move:
		{
			ImGui::Text("Enemy action: move");
		}
		break;
		case Command::attack:
		{
			ImGui::Text("Enemy action: attack");
		}
		break;
	}

	switch (enemy_cmd.dir)
	{
		case Board::Left:
			ImGui::Text("Enemy dir: Left");
			break;
		case Board::Right:
			ImGui::Text("Enemy dir: Right");
			break;
		case Board::Down:
			ImGui::Text("Enemy dir: Down");
			break;
		case Board::Up:
			ImGui::Text("Enemy dir: Up");
			break;
	}
	int player_health = 0;
	for (int i = 0; i < board.w_ops_count; ++i)
	{
		player_health += board.gs.white_ops[i].get_cost();
	}
	int enemy_health = 0;
	for (int i = 0; i < board.b_ops_count; ++i)
	{
		enemy_health += board.gs.black_ops[i].get_cost();
	}
	ImGui::Text("Health: %d : %d", player_health, enemy_health);
	// ImGui::ShowDemoWindow();
	ImGui::End();
}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
