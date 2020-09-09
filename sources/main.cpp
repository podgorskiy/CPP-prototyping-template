#include "Application.h"
#include <GL/gl3w.h>

#include <GLFW/glfw3.h>
#include <memory>
#include <chrono>
#include "imgui.h"
#include "examples/imgui_impl_glfw.h"
#include "examples/imgui_impl_opengl3.h"

int main(int argc, const char* const* argv)
{
	GLFWwindow* window;

	/* Initialize the library */
	glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

	/* Create a windowed mode window and its OpenGL context */
	window = glfwCreateWindow(640, 640, "Hello World", NULL, NULL);

	/* Make the window's context current */
	glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

	gl3wInit();

	const char* OpenGLversion = (const char*)glGetString(GL_VERSION);
	const char* GLSLversion = (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION);

	printf("OpenGL %s GLSL: %s", OpenGLversion, GLSLversion);
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);

    ImGui::CreateContext();

	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window, false);

	{
		std::shared_ptr<Application> app = std::make_shared<Application>(argc, argv);

	    const char* glsl_version = "#version 130";
	    ImGui_ImplOpenGL3_Init(glsl_version);

		int width, height, display_w, display_h;
		glfwGetWindowSize(window, &width, &height);
		glfwGetFramebufferSize(window, &display_w, &display_h);

		app->Resize(width, height, display_w, display_h);
		
		glfwSetWindowUserPointer(window, app.get());

		glfwSetWindowSizeCallback(window, [](GLFWwindow* window, int width, int height)
		{
			auto app = static_cast<Application*>(glfwGetWindowUserPointer(window));
		    int display_w, display_h;
		    glfwGetFramebufferSize(window, &display_w, &display_h);
			app->Resize(width, height, display_w, display_h);
		});

		glfwSetKeyCallback(window, [](GLFWwindow*, int key, int, int action, int mods)
		{
			ImGuiIO& io = ImGui::GetIO();
			if (action == GLFW_PRESS)
				io.KeysDown[key] = true;
			if (action == GLFW_RELEASE)
				io.KeysDown[key] = false;

			io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
			io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
			io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
			io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];
		});

		glfwSetCharCallback(window, [](GLFWwindow*, unsigned int c)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.AddInputCharacter((unsigned short)c);
		});

		glfwSetScrollCallback(window, [](GLFWwindow*, double /*xoffset*/, double yoffset)
		{
			ImGuiIO& io = ImGui::GetIO();
			io.MouseWheel += (float)yoffset * 2.0f;
		});

		glfwSetMouseButtonCallback(window, [](GLFWwindow*, int button, int action, int /*mods*/)
		{
			ImGuiIO& io = ImGui::GetIO();

			if (button >= 0 && button < 3)
			{
				io.MouseDown[button] = action == GLFW_PRESS;
			}
		});

		auto start = std::chrono::steady_clock::now();

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			auto current_timestamp = std::chrono::steady_clock::now();

			std::chrono::duration<float> elapsed_time = (current_timestamp - start);

	        ImGui_ImplOpenGL3_NewFrame();
	        ImGui_ImplGlfw_NewFrame();
	        ImGui::NewFrame();

		    glfwGetFramebufferSize(window, &display_w, &display_h);

			glViewport(0, 0, display_w, display_h);
			glClear(GL_COLOR_BUFFER_BIT);

			app->Draw(elapsed_time.count());

			ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		glfwSetWindowSizeCallback(window, nullptr);
		ImGui_ImplGlfw_Shutdown();
	}

	glfwTerminate();
	return 0;
}
