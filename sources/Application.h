#pragma once
#include <glm/glm.hpp>
#include <chessis/board.h>

class Application
{
public:
	Application(int argc, const char* const* argv);
	~Application();

	void Draw(float time);
	void Resize(int width, int height, int display_w, int display_h);

private:
	chessis::Board board;
	glm::vec3 m_color = glm::vec3(0.f);
};
