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
		int windowWidth = 800;
		int windowHeight = 600;
		int windowPosX = 100;
		int windowPosY = 100;
		bool windowMaximized = false;
		mINI::INIStructure ini;

		void ErrorCallback([[maybe_unused]] int error, const char * description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}

		void KeyCallback(GLFWwindow * window, int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
		{
			if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
				glfwSetWindowShouldClose(window, GLFW_TRUE);
		}

		void WindowPosCallback([[maybe_unused]] GLFWwindow * window, int x, int y)
		{		
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				windowPosX = x;
				windowPosY = y;
			}
		}

		void WindowSizeCallback([[maybe_unused]]GLFWwindow * window, int width, int height)
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

		void GetConfig()
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder("ImFrame", "Feature");
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

		void SaveConfig()
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder("ImFrame", "Feature");
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

    void RunImFrame()
    {
		GetConfig();

		glfwSetErrorCallback(ErrorCallback);

		if (!glfwInit())
			exit(EXIT_FAILURE);

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

		GLFWwindow * window = glfwCreateWindow(windowWidth, windowHeight, "ImFrame Test", NULL, NULL);
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
		gladLoadGL();
		glfwSwapInterval(1);

		InitDemo();

		while (!glfwWindowShouldClose(window))
		{
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);

			glViewport(0, 0, width, height);
			glClear(GL_COLOR_BUFFER_BIT);

			UpdateDemo(window);

			glfwSwapBuffers(window);
			glfwPollEvents();
		}

		SaveConfig();

		glfwDestroyWindow(window);
		glfwTerminate();
    }

}
