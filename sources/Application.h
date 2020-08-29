#pragma once
#include <glm/glm.hpp>

class Application
{
public:
	Application();
	~Application();

	void Draw(float time);
	void Resize(int width, int height, int display_w, int display_h);

private:
	glm::vec3 m_color = glm::vec3(0.f);
};
