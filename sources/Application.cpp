#include "Application.h"
#include <GL/gl3w.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "imgui.h"
#include "examples/imgui_impl_opengl3.h"

GLuint CompileShader(const char* src, GLint type)
{
	GLuint shader = glCreateShader(type);

	glShaderSource(shader, 1, &src, NULL);

	glCompileShader(shader);
	GLint compiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &compiled);
	GLint infoLen = 0;
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLen);

	if (infoLen > 1)
	{
		printf("%s during shader compilation.\n ", compiled == GL_TRUE ? "Warning" : "Error");
		char* buf = new char[infoLen];
		glGetShaderInfoLog(shader, infoLen, NULL, buf);
		printf("Compilation log: %s\n", buf);
		delete[] buf;
	}
	
	return shader;
}

Application::Application()
{
	gl3wInit();

	const char* OpenGLversion = (const char*)glGetString(GL_VERSION);
	const char* GLSLversion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("OpenGL %s GLSL: %s", OpenGLversion, GLSLversion);


	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

	const char* vertex_shader_src = R"(
		attribute vec2 a_position;
		uniform mat4 u_transform;

		void main()
		{
			gl_Position = u_transform * vec4(a_position, 0.0, 1.0);
		}
	)";

	const char* fragment_shader_src = R"(
		uniform vec3 u_color;

		void main()
		{
			gl_FragColor = vec4(u_color, 1.0);
		}
	)";

	int vertex_shader_handle = CompileShader(vertex_shader_src, GL_VERTEX_SHADER);
	int fragment_shader_handle = CompileShader(fragment_shader_src, GL_FRAGMENT_SHADER);

	m_program = glCreateProgram();

	glAttachShader(m_program, vertex_shader_handle);
	glAttachShader(m_program, fragment_shader_handle);

	glLinkProgram(m_program);

	int linked;
	glGetProgramiv(m_program, GL_LINK_STATUS, &linked);
	if (!linked)
	{
		GLint infoLen = 0;
		glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
		if (infoLen > 1)
		{
			char* buf = new char[infoLen];
			glGetProgramInfoLog(m_program, infoLen, NULL, buf);
			printf("Linking error: \n%s\n", buf);
			delete[] buf;
		}
	}

	glDetachShader(m_program, vertex_shader_handle);
	glDetachShader(m_program, fragment_shader_handle);

	glDeleteShader(vertex_shader_handle);
	glDeleteShader(fragment_shader_handle);

	m_attrib_pos = glGetAttribLocation(m_program, "a_position");
	m_uniform_transform = glGetUniformLocation(m_program, "u_transform");
	m_uniform_color = glGetUniformLocation(m_program, "u_color");

	glGenBuffers(1, &m_vertexBufferObject);
	glGenBuffers(1, &m_indexBufferObject);

	glm::vec2 vertices[3];
	vertices[0] = glm::vec2(-1.0, -1.0);
	vertices[1] = glm::vec2(1.0, -1.0);
	vertices[2] = glm::vec2(0.0, 1.0);

	short indices[3] = {0, 1, 2};

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(glm::vec2), vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 3 * sizeof(short), indices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	m_triangleRotation = 0;
}


Application::~Application()
{
	glDeleteProgram(m_program);
}

inline void* ToVoidPointer(int offset)
{
	size_t offset_ = static_cast<size_t>(offset);
	return reinterpret_cast<void*>(offset_);
}

void Application::Draw(float time)
{
	ImGui::Begin("IntroductionToComputerGraphics");
	ImGui::Text("Example of using ImGUI.");
	ImGui::SliderFloat("Rotation", &m_triangleRotation, 0, 2.0 * glm::pi<float>());
	ImGui::ColorEdit3("Color", &m_color[0]);

	if (ImGui::Button("Reset"))
	{
		m_triangleRotation = 0.0f;
		m_color = glm::vec3(1.0, 0.0, 0.0);
	}

	ImGui::End();

	ImGui::ShowDemoWindow();

	glm::mat4 rotation = glm::rotate(glm::mat4(1.0), m_triangleRotation, glm::vec3(0, 0, 1.0f));
	glUniform3fv(m_uniform_color, 1, &m_color[0]);

	glViewport(0, 0, m_width, m_height);

	float aspect = m_width / (float)m_height;

	float view_height = 2.2f;
	float view_width = aspect * view_height;

	float left = -view_width / 2.0f;
	float right = view_width / 2.0f;
	float top = view_height / 2.0f;
	float bottom = - view_height / 2.0f;

	glm::mat4 transform = glm::ortho(left, right, bottom, top, -1.0f, 1.0f) * rotation;

	glUniformMatrix4fv(m_uniform_transform, 1, GL_FALSE, &transform[0][0]);

	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(m_program);

	glBindBuffer(GL_ARRAY_BUFFER, m_vertexBufferObject);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBufferObject);

	glEnableVertexAttribArray(m_attrib_pos);
	glVertexAttribPointer(m_attrib_pos, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), ToVoidPointer(0));

	glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_SHORT, 0);

	glDisableVertexAttribArray(m_attrib_pos);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}

void Application::Resize(int width, int height)
{
	m_width = width;
	m_height = height;
}
