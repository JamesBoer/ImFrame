/*
The MIT License (MIT)

Copyright (c) 2021 James Boer

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "ImfInternal.h"

namespace ImFrame
{
	namespace
	{

		// Persistent app data
		int windowWidth = 800;
		int windowHeight = 600;
		int windowPosX = 100;
		int windowPosY = 100;
		bool windowMaximized = false;
		mINI::INIStructure ini;
		ImAppPtr appPtr;

		void ErrorCallback([[maybe_unused]] int error, const char * description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}

		void KeyCallback([[maybe_unused]] GLFWwindow * window, int key, int scancode, int action, int mods)
		{
			appPtr->OnKeyEvent(key, scancode, action, mods);
		}

		void WindowPosCallback(GLFWwindow * window, int x, int y)
		{		
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				windowPosX = x;
				windowPosY = y;
			}
		}

		void WindowSizeCallback(GLFWwindow * window, int width, int height)
		{
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				windowWidth = width;
				windowHeight = height;
			}
		}

		void WindowMaximizeCallback([[maybe_unused]] GLFWwindow * window, int maximized)
		{
			windowMaximized = maximized ? true : false;
		}

		void GetConfig(const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append("settings.ini");
			mINI::INIFile file(configFolder.string());
			file.read(ini);
			std::string ww = ini["window"]["width"];
			if (!ww.empty())
				windowWidth = std::stoi(ww);
			std::string wh = ini["window"]["height"];
			if (!wh.empty())
				windowHeight = std::stoi(wh);
			std::string wpx = ini["window"]["posx"];
			if (!wpx.empty())
				windowPosX = std::stoi(wpx);
			std::string wpy = ini["window"]["posy"];
			if (!wpy.empty())
				windowPosY = std::stoi(wpy);
			std::string wm = ini["window"]["maximized"];
			if (!wm.empty())
				windowMaximized = std::stoi(wm) == 0 ? false : true;
		}

		void SaveConfig(const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append("settings.ini");
			mINI::INIFile file(configFolder.string());
			ini["window"]["width"] = std::to_string(windowWidth);
			ini["window"]["height"] = std::to_string(windowHeight);
			ini["window"]["posx"] = std::to_string(windowPosX);
			ini["window"]["posy"] = std::to_string(windowPosY);
			ini["window"]["maximized"] = std::to_string(windowMaximized ? 1 : 0);
			file.write(ini, true);
		}

	}

	std::string GetVersionString()
	{
		char buffer[32];
		snprintf(buffer, std::size(buffer), "%i.%i.%i", ImFrame::MajorVersion, ImFrame::MinorVersion, ImFrame::PatchNumber);
		return buffer;
	}

    void RunImFrame(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn)
    {
		namespace fs = std::filesystem;

		// Read existing config data
		GetConfig(orgName, appName);

		// Init GLFW and create window, setup callbacks, etc
		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit())
			exit(EXIT_FAILURE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, appName.c_str(), NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			exit(EXIT_FAILURE);
		}
		glfwSetWindowPosCallback(window, WindowPosCallback);
		glfwSetWindowSizeCallback(window, WindowSizeCallback);
		glfwSetWindowMaximizeCallback(window, WindowMaximizeCallback);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetWindowPos(window, windowPosX, windowPosY);
		if (windowMaximized)
			glfwMaximizeWindow(window);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Initialize glad GL functions
		gladLoadGL();

		// Initialize ImGui
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		fs::path iniPath = GetConfigFolder(orgName, appName);
		iniPath.append("imgui.ini");
		auto iniStr = iniPath.string();
		io.IniFilename = iniStr.c_str();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init();
		ImGui_ImplOpenGL3_CreateFontsTexture();

		// Initialize ImPlot
		ImPlot::CreateContext();

		// Create user-defined app
		appPtr = createAppFn(window);

		// Main application loop
		while (!glfwWindowShouldClose(window))
		{
			// Perform event and input polling
			glfwPollEvents();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Clear render buffer
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT);

			// Perform app-specific updates
			appPtr->OnUpdate();

			// Render ImGui to draw data
			ImGui::Render();

			// Render ImGui
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Present buffer
			glfwSwapBuffers(window);
		}

		// Delete application
		appPtr = nullptr;

		// Save config data to disk
		SaveConfig(orgName, appName);

		// Shut down ImGui and ImPlot
		ImGui_ImplOpenGL3_DestroyFontsTexture();
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		// Shut down glfw
		glfwDestroyWindow(window);
		glfwTerminate();
    }

}
