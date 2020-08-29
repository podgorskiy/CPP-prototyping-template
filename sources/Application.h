#pragma once
#include <glm/glm.hpp>

class Application
{
public:
	Application();
	~Application();

	void Draw(float time);
	void Resize(int width, int height);

private:
	unsigned int m_program;
	unsigned int m_attrib_pos;

	unsigned int m_uniform_transform;
	unsigned int m_uniform_color;

	unsigned int m_vertexBufferObject;
	unsigned int m_indexBufferObject;

	glm::vec3 m_color;

	int m_width;
	int m_height;
	float m_triangleRotation;
};
