#include "Application.h"
#include <imgui.h>
#include <examples/imgui_impl_opengl3.h>
#include <stdio.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>



Application::Application()
{
}


Application::~Application()
{
}

void Application::Draw(float time)
{
	ImGui::Begin("IntroductionToComputerGraphics");
	ImGui::Text("Example of using ImGUI.");
	ImGui::ColorEdit3("Color", &m_color[0]);

	ImGui::End();

	// ImGui::ShowDemoWindow();
}

void Application::Resize(int width, int height, int display_w, int display_h)
{
}
