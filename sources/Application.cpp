#include "Application.h"
#include "utils/buffer.h"
#include "utils/stack_buffer.h"
#include "chessis/misc.h"
#include "chessis/loader.h"
#include "chessis/debug.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <string.h>
#include <fsal.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <bits/stdc++.h>


using namespace chessis;



int leaves = 0;



Application::Application()
{
	static_assert(sizeof(Piece) == sizeof(uint32_t));

	fsal::FileSystem fs;
	std::string b = fs.Open("../board.txt");
	chessis::make_board(board, b);
}


Application::~Application()
{
}

void Application::Draw(float time)
{
	static Turn::Enum turn = Turn::WhitePLay;

	chessis::DebugUI(board, turn);

}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
