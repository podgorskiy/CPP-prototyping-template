#include "Application.h"
#include "utils/buffer.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <string.h>
#include <fsal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


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
			case Operator::Pawn: return 10 * health;
			case Operator::Knight: return 20 * health;
			case Operator::Rook: return 30 * health;
		}
		return 0;
	}
};
#pragma pack(pop)

Operator noop({Operator::Dead, 0, 0, 0, 0, 0});


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

	glm::ivec2 size;
	buffer<int> cell_state;

	bool valid_coord(int x, int y) const { return !(x < 0 || y < 0 || x >= size.x || y >= size.y); }
	const int get_cell(int x, int y) const { return valid_coord(x, y) ? cell_state[x + y * size.x] : Block; }
	void set_cell(int x, int y, int n) { if (valid_coord(x, y)) cell_state[x + y * size.x] = n; }
	void set_cell_and_or(int x, int y, int a, int o) { if (valid_coord(x, y)) { auto& c = cell_state[x + y * size.x]; c &= a; c |= o; }}
	void clear_cell(int x, int y) { set_cell_and_or(x, y, 0xFF, 0); }
	void set_cell_op(int x, int y, int op_id, bool for_black) { set_cell_and_or(x, y, 0xFF, (op_id << 8) | (for_black ? Board::HasB:Board::HasW)); }


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

int Evaluate(const Board& board, const GameState& gs, bool for_black)
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
				distance += d * (for_black ? (gs.white_ops[i].health / gs.black_ops[j].health + 1) : (gs.black_ops[j].health / gs.white_ops[i].health + 1));
			}
		}
		ew += gs.white_ops[i].get_cost();
	}

	if (ew == 0) ew = -1000000;
	if (eb == 0) eb = 1000000;
	int e = ew + eb;
	return (for_black ? -e : e) - distance;
	return distance;
}

struct Command
{
	enum Action
	{
		ml, mr, md, mu,
		al, ar, ad, au,
		end
	};
	int new_eval = 0;
	Action action = end;
	int op_id = -1;

	Command(int new_eval): new_eval(new_eval), action(end), op_id(-1) {}
	Command(int new_eval, Action action, int op_id): new_eval(new_eval), action(action), op_id(op_id) {}
};

Command TraverseState(Board& board, GameState& gs, bool for_black, int depth)
{
	if (depth == 0)
	{
		return Command(Evaluate(board, gs, for_black));
	}
	Operator* ops = !for_black ? gs.white_ops  : gs.black_ops;
	int count = !for_black ? board.w_ops_count  : board.b_ops_count;

	Command::Action action_for_op[12];
	int ev_action_for_op[12];

	for (int i = 0; i < count; ++i)
	{
		Operator& op = ops[i];

		if (op.type == Operator::Dead)
			continue;

		int ev[8] = {-1000000, -1000000, -1000000, -1000000, -1000000, -1000000, -1000000, -1000000};

		// move left
		if (board.get_cell(op.x - 1, op.y) == Board::Walkable)
		{
			board.clear_cell(op.x, op.y);
			--op.x;
			board.set_cell_op(op.x, op.y, i, for_black);
			ev[Command::ml] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			board.clear_cell(op.x, op.y);
			++op.x;
			board.set_cell_op(op.x, op.y, i, for_black);
		}

		// move right
		if (board.get_cell(op.x + 1, op.y) == Board::Walkable)
		{
			board.clear_cell(op.x, op.y);
			++op.x;
			board.set_cell_op(op.x, op.y, i, for_black);
			ev[Command::mr] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			board.clear_cell(op.x, op.y);
			--op.x;
			board.set_cell_op(op.x, op.y, i, for_black);
		}

		// move down
		if (board.get_cell(op.x, op.y  +1) == Board::Walkable)
		{
			board.clear_cell(op.x, op.y);
			++op.y;
			board.set_cell_op(op.x, op.y, i, for_black);
			ev[Command::md] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			board.clear_cell(op.x, op.y);
			--op.y;
			board.set_cell_op(op.x, op.y, i, for_black);
		}

		// move up
		if (board.get_cell(op.x, op.y - 1) == Board::Walkable)
		{
			board.clear_cell(op.x, op.y);
			--op.y;
			board.set_cell_op(op.x, op.y, i, for_black);
			ev[Command::mu] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			board.clear_cell(op.x, op.y);
			++op.y;
			board.set_cell_op(op.x, op.y, i, for_black);
		}

		auto& opl = board.get_opp(gs, op.x - 1, op.y + 0, for_black, !for_black);
		auto& opr = board.get_opp(gs, op.x + 1, op.y + 0, for_black, !for_black);
		auto& opd = board.get_opp(gs, op.x + 0, op.y + 1, for_black, !for_black);
		auto& opu = board.get_opp(gs, op.x + 0, op.y - 1, for_black, !for_black);

		// attack left
		if (opl.type != 0)
		{
			Operator backup = opl;
			opl.health -= 1;
			if (opl.health < 0)
			{
				opl.health = 0;
				opl.type = Operator::Dead;
			}

			ev[Command::al] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			opl = backup;
		}

		// attack right
		if (opr.type != 0)
		{
			Operator backup = opr;
			opr.health -= 1;
			if (opr.health < 0)
			{
				opr.health = 0;
				opr.type = Operator::Dead;
			}

			ev[Command::ar] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			opr = backup;
		}

		// attack down
		if (opd.type != 0)
		{
			Operator backup = opd;
			opd.health -= 1;
			if (opd.health < 0)
			{
				opd.health = 0;
				opd.type = Operator::Dead;
			}

			ev[Command::ad] = -TraverseState(board, gs, !for_black, depth-1).new_eval;
			opd = backup;
		}

		// attack up
		if (opu.type != 0)
		{
			Operator backup = opu;
			opu.health -= 1;
			if (opu.health < 0)
			{
				opu.health = 0;
				opu.type = Operator::Dead;
			}

			ev[Command::au] =  -TraverseState(board, gs, !for_black, depth-1).new_eval;
			opu = backup;
		}
		int max = ev[Command::ml];
		Command::Action max_aid = Command::ml;
		for (int j = 1; j < 8; ++j)
		{
			if (ev[j] > max)
			{
				max = ev[j];
				max_aid = (Command::Action)j;
			}
		}

		action_for_op[i] = max_aid;
		ev_action_for_op[i] = max;
	}

	int max = ev_action_for_op[0];
	Command::Action max_aid = action_for_op[0];
	int max_op = 0;

	for (int i = 1; i < count; ++i)
	{
		if (ev_action_for_op[i] > max && ops[i].type != Operator::Dead)
		{
			max_op = i;
			max_aid = action_for_op[i];
			max = ev_action_for_op[i];
		}
	}
	if (max == -1000000)
		return Command(Evaluate(board, gs, for_black));
	return Command((max * 90) / 100, max_aid, max_op);
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
					case 'p': board.gs.black_ops[board.b_ops_count] = Operator({Operator::Pawn, 3, 0, uint8_t(j), uint8_t(i), 0});
						board.cell_state.data()[i * board.size.x + j] |= Board::HasB | (board.b_ops_count++ << 8);
						break;
					case 'P': board.gs.white_ops[board.w_ops_count] = Operator({Operator::Pawn, 3,  0, uint8_t(j), uint8_t(i), 0});
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

void MakeMove(Command tr, bool for_black)
{
	Operator* ops = !for_black ? board.gs.white_ops : board.gs.black_ops;
	Operator& op = ops[tr.op_id];
	Operator* f = nullptr;
	switch (tr.action)
	{
		case Command::ml:
			board.clear_cell(op.x, op.y);
			--op.x;
			board.set_cell_op(op.x, op.y, tr.op_id, for_black);
			break;
		case Command::mr:
			board.clear_cell(op.x, op.y);
			++op.x;
			board.set_cell_op(op.x, op.y, tr.op_id, for_black);
			break;
		case Command::md:
			board.clear_cell(op.x, op.y);
			++op.y;
			board.set_cell_op(op.x, op.y, tr.op_id, for_black);
			break;
		case Command::mu:
			board.clear_cell(op.x, op.y);
			--op.y;
			board.set_cell_op(op.x, op.y, tr.op_id, for_black);
			break;

		case Command::al:
			f = &board.get_opp(board.gs, op.x - 1, op.y + 0, true, true);
			f->health--;
			break;
		case Command::ar:
			f = &board.get_opp(board.gs, op.x + 1, op.y + 0, true, true);
			f->health--;
			break;
		case Command::ad:
			f = &board.get_opp(board.gs, op.x + 0, op.y + 1, true, true);
			f->health--;
			break;
		case Command::au:
			f = &board.get_opp(board.gs, op.x + 0, op.y - 1, true, true);
			f->health--;
			break;
	}
	if (f && f->health <= 0)
	{
		f->type = Operator::Dead;
		board.clear_cell(f->x, f->y);
	}
}

Command InferCommand(int op_id, bool for_black, int x, int y)
{
	Operator* ops = !for_black ? board.gs.white_ops : board.gs.black_ops;
	Operator& op = ops[op_id];
	if (board.valid_coord(x, y))
	{
		if (board.get_cell(x, y) == Board::Walkable)
		{
			if (x == op.x + 1 && y == op.y) return Command(0, Command::mr, op_id);
			else if (x == op.x - 1 && y == op.y) return Command(0, Command::ml, op_id);
			else if (x == op.x && y == op.y - 1) return Command(0, Command::mu, op_id);
			else if (x == op.x && y == op.y + 1) return Command(0, Command::md, op_id);
		}
		else
		{
			const Operator& enemy = board.get_opp(board.gs, x, y, for_black, !for_black);
			if (enemy.type != Operator::Dead)
			{
				if (x == op.x + 1 && y == op.y) return Command(0, Command::ar, op_id);
				else if (x == op.x - 1 && y == op.y) return Command(0, Command::al, op_id);
				else if (x == op.x && y == op.y - 1) return Command(0, Command::au, op_id);
				else if (x == op.x && y == op.y + 1) return Command(0, Command::ad, op_id);
			}
		}
	}
	return Command(0, Command::end, -1);
}

void Application::Draw(float time)
{
	static bool for_black = false;

	ImGui::Begin("Board");

	ImGui::Columns(2);

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

			if (!for_black)
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
						Command cmd = InferCommand(op_id, false, j, i);
						if (cmd.action != Command::end)
						{
							if (ImGui::AcceptDragDropPayload("op"))
							{
								MakeMove(cmd, false);
								for_black = !for_black;
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

	ImGui::Text("Evaluation: %d", Evaluate(board, board.gs, false));

	if (ImGui::Button("Make move") || for_black)
	{
		Command tr = TraverseState(board, board.gs, for_black, 12);
		MakeMove(tr, for_black);
		for_black = !for_black;
	}

	ImGui::End();

	// ImGui::ShowDemoWindow();
}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
