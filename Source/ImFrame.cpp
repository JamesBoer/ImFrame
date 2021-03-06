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

#include "Fonts/CarlitoRegular.h"
#include "Fonts/OpenSansRegular.h"
#include "Fonts/OpenSansSemiBold.h"
#include "Fonts/RobotoMedium.h"
#include "Fonts/RobotoRegular.h"

namespace ImFrame
{
	namespace
	{

		// Persistent app data

		// Window data
		int s_windowWidth = 800;
		int s_windowHeight = 600;
		int s_windowPosX = 100;
		int s_windowPosY = 100;
		bool s_windowMaximized = false;
		std::array<float, 3> s_backgroundColor = { 0.08f, 0.08f, 0.08f };

		// ImGui settings
		bool s_fontEnabled = true;
		FontType s_fontType = FontType::RobotoRegular;
		float s_fontSize = 15.0f;

		ImAppPtr s_appPtr;

		bool s_fontChanged = true;
		ImFont * s_customFont;

		void ErrorCallback([[maybe_unused]] int error, const char * description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}

		void KeyCallback([[maybe_unused]] GLFWwindow * window, int key, int scancode, int action, int mods)
		{
			if (s_appPtr)
				s_appPtr->OnKeyEvent(key, scancode, action, mods);
		}

		void WindowPosCallback(GLFWwindow * window, int x, int y)
		{		
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				s_windowPosX = x;
				s_windowPosY = y;
			}
			if (s_appPtr)
				s_appPtr->OnWindowPositionChange(x, y);
		}

		void WindowSizeCallback(GLFWwindow * window, int width, int height)
		{
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				s_windowWidth = width;
				s_windowHeight = height;
			}
			if (s_appPtr)
				s_appPtr->OnWindowSizeChange(width, height);
		}

		void WindowMaximizeCallback([[maybe_unused]] GLFWwindow * window, int maximized)
		{
			s_windowMaximized = maximized ? true : false;
			if (s_appPtr)
				s_appPtr->OnWindowMaximize(s_windowMaximized);
		}

		void WindowMouseButtonCallback([[maybe_unused]] GLFWwindow * window, int button, int action, int mods)
		{
			if (s_appPtr)
				s_appPtr->OnMouseButtonEvent(button, action, mods);
		}

		void WindowCursorPositionCallback([[maybe_unused]] GLFWwindow * window, double x, double y)
		{
			if (s_appPtr)
				s_appPtr->OnCursorPosition(x, y);
		}

		float GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, float defaultValue)
		{
			auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stof(s);
		}

		int GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, int defaultValue)
		{
			auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stoi(s);
		}

		bool GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, bool defaultValue)
		{
			auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stoi(s) == 0 ? false : true;
		}

		void GetConfig(mINI::INIStructure & ini, const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append("settings.ini");
			mINI::INIFile file(configFolder.string());
			file.read(ini);
			s_windowWidth = GetConfigValue(ini, "window", "width", s_windowWidth);
			s_windowHeight = GetConfigValue(ini, "window", "height", s_windowHeight);
			s_windowPosX = GetConfigValue(ini, "window", "posx", s_windowPosX);
			s_windowPosY = GetConfigValue(ini, "window", "posy", s_windowPosY);
			s_windowMaximized = GetConfigValue(ini, "window", "maximized", s_windowMaximized);
			s_backgroundColor[0] = GetConfigValue(ini, "window", "bgcolorr", s_backgroundColor[0]);
			s_backgroundColor[1] = GetConfigValue(ini, "window", "bgcolorg", s_backgroundColor[1]);
			s_backgroundColor[2] = GetConfigValue(ini, "window", "bgcolorb", s_backgroundColor[2]);
			s_fontEnabled = GetConfigValue(ini, "font", "enabled", s_fontEnabled);
			s_fontType = static_cast<ImFrame::FontType>(GetConfigValue(ini, "font", "type", static_cast<int>(s_fontType)));
			s_fontSize = GetConfigValue(ini, "font", "size", s_fontSize);
		}

		void SaveConfig(mINI::INIStructure & ini, const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append("settings.ini");
			mINI::INIFile file(configFolder.string());
			ini["window"]["width"] = std::to_string(s_windowWidth);
			ini["window"]["height"] = std::to_string(s_windowHeight);
			ini["window"]["posx"] = std::to_string(s_windowPosX);
			ini["window"]["posy"] = std::to_string(s_windowPosY);
			ini["window"]["maximized"] = std::to_string(s_windowMaximized ? 1 : 0);
			ini["window"]["bgcolorr"] = std::to_string(s_backgroundColor[0]);
			ini["window"]["bgcolorg"] = std::to_string(s_backgroundColor[1]);
			ini["window"]["bgcolorb"] = std::to_string(s_backgroundColor[2]);
			ini["font"]["enabled"] = std::to_string(s_fontEnabled ? 1 : 0);
			ini["font"]["type"] = std::to_string(static_cast<int>(s_fontType));
			ini["font"]["size"] = std::to_string(s_fontSize);
			file.write(ini);
		}

		void OnExit()
		{
#ifdef IMFRAME_WINDOWS

			// Assert that the function returns zero indicating no memory leaks from
			// the debug CRT libraries.
			assert(!_CrtDumpMemoryLeaks() && "Memory leak detected!");
#endif
		}

		void UpdateCustomFont()
		{
			if (!s_fontChanged)
				return;
			if (s_fontEnabled)
			{
				ImGuiIO & io = ImGui::GetIO();
				if (s_customFont)
					io.Fonts->Clear();
				switch (s_fontType)
				{
					case FontType::CarlitoRegular:
						s_customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&CarlitoRegular_compressed_data[0]), CarlitoRegular_compressed_size, s_fontSize);
						break;
					case FontType::OpenSansRegular:
						s_customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&OpenSansRegular_compressed_data[0]), OpenSansRegular_compressed_size, s_fontSize);
						break;
					case FontType::OpenSansSemiBold:
						s_customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&OpenSansSemiBold_compressed_data[0]), OpenSansSemiBold_compressed_size, s_fontSize);
						break;
					case FontType::RobotoMedium:
						s_customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&RobotoMedium_compressed_data[0]), RobotoMedium_compressed_size, s_fontSize);
						break;
					case FontType::RobotoRegular:
						s_customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&RobotoRegular_compressed_data[0]), RobotoRegular_compressed_size, s_fontSize);
						break;
				}
				ImGui_ImplOpenGL3_CreateFontsTexture();
			}
			else
			{
				s_customFont = nullptr;
			}
			s_fontChanged = false;
		}
	}

	std::string GetVersionString()
	{
		char buffer[32];
		snprintf(buffer, std::size(buffer), "%i.%i.%i", ImFrame::MajorVersion, ImFrame::MinorVersion, ImFrame::PatchNumber);
		return buffer;
	}

	std::optional<std::filesystem::path> OpenFileDialog(const char * filters, const char * defaultPath)
	{
		nfdchar_t * outPath = NULL;
		nfdresult_t result = NFD_OpenDialog(filters, defaultPath, &outPath);
		if (result == NFD_OKAY)
		{
			std::string outStr = outPath;
			free(outPath);
			return outStr;
		}
		else if (result == NFD_CANCEL)
		{
			return std::optional<std::filesystem::path>();
		}
		else
		{
			//NFD_GetError() gets last error;
			return std::optional<std::filesystem::path>();
		}
	}

	std::optional<std::vector<std::filesystem::path>> OpenFilesDialog(const char * filters, const char * defaultPath)
	{
		nfdpathset_t pathSet;
		nfdresult_t result = NFD_OpenDialogMultiple(filters, defaultPath, &pathSet);
		if (result == NFD_OKAY)
		{
			std::vector<std::filesystem::path> paths;
			for (size_t i = 0; i < NFD_PathSet_GetCount(&pathSet); ++i)
			{
				std::string path = NFD_PathSet_GetPath(&pathSet, i);
				paths.emplace_back(path);
			}
			NFD_PathSet_Free(&pathSet);
			return paths;
		}
		else if (result == NFD_CANCEL)
		{
			return std::optional<std::vector<std::filesystem::path>>();
		}
		else
		{
			//NFD_GetError() gets last error;
			return std::optional<std::vector<std::filesystem::path>>();
		}
	}

	std::optional<std::filesystem::path> SaveFileDialog(const char * filters, const char * defaultPath)
	{
		nfdchar_t * savePath = NULL;
		nfdresult_t result = NFD_SaveDialog(filters, defaultPath, &savePath);
		if (result == NFD_OKAY)
		{
			std::string saveStr = savePath;
			free(savePath);
			return saveStr;
		}
		else if (result == NFD_CANCEL)
		{
			return std::optional<std::filesystem::path>();
		}
		else
		{
			//NFD_GetError() gets last error;
			return std::optional<std::filesystem::path>();
		}
	}

	std::optional<std::filesystem::path> PickFolderDialog(const char * defaultPath)
	{
		nfdchar_t * outPath = NULL;
		nfdresult_t result = NFD_PickFolder(defaultPath, &outPath);
		if (result == NFD_OKAY)
		{
			std::string folderStr = outPath;
			free(outPath);
			return folderStr;
		}
		else if (result == NFD_CANCEL)
		{
			return std::optional<std::filesystem::path>();
		}
		else
		{
			//NFD_GetError() gets last error;
			return std::optional<std::filesystem::path>();
		}
	}

	bool IsCustomFontEnabled()
	{
		return s_fontEnabled;
	}

	void EnableCustomFont(bool enable)
	{
		if (enable != s_fontEnabled)
		{
			s_fontEnabled = enable;
			s_fontChanged = true;
		}
	}

	FontType GetCustomFontType()
	{
		return s_fontType;
	}

	void SetCustomFontType(FontType font)
	{
		if (font != s_fontType)
		{
			s_fontType = font;
			s_fontChanged = true;
		}
	}

	float GetCustomFontSize()
	{
		return s_fontSize;
	}

	void SetCustomFontSize(float pixelSize)
	{
		if (pixelSize != s_fontSize)
		{
			s_fontSize = pixelSize;
			s_fontChanged = true;
		}
	}

	void SetBackgroundColor(std::array<float, 3> color)
	{
		s_backgroundColor = color;
	}

	std::array<float, 3> GetBackgroundColor()
	{
		return s_backgroundColor;
	}

    int Run(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn)
    {
		namespace fs = std::filesystem;

		// Signal this function to execute on exit
		atexit(OnExit);

#ifdef IMFRAME_WINDOWS
		// Enable memory leak checking
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

		// Read existing config data
		mINI::INIStructure ini;
		GetConfig(ini, orgName, appName);

		// Init GLFW and create window
		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit())
			return 1;
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow * window = glfwCreateWindow(s_windowWidth, s_windowHeight, appName.c_str(), NULL, NULL);
		if (!window)
		{
			glfwTerminate();
			return 1;
		}

		// Set up window callbacks
		glfwSetWindowPosCallback(window, WindowPosCallback);
		glfwSetWindowSizeCallback(window, WindowSizeCallback);
		glfwSetWindowMaximizeCallback(window, WindowMaximizeCallback);
		glfwSetKeyCallback(window, KeyCallback);
		glfwSetMouseButtonCallback(window, WindowMouseButtonCallback);
		glfwSetCursorPosCallback(window, WindowCursorPositionCallback);
		glfwSetWindowPos(window, s_windowPosX, s_windowPosY);
		if (s_windowMaximized)
			glfwMaximizeWindow(window);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Initialize glad GL functions
		gladLoadGL();

		// Initialize ImGui
		ImGui::CreateContext();
		ImGuiIO & io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
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
		s_appPtr = createAppFn(window);

		// TEMP TEST!!!
		auto filePath = GetExecutableFolder();
		filePath.append("world.png");
		if (fs::exists(filePath))
		{
			GLFWimage icons[1];
			icons[0].pixels = SOIL_load_image(filePath.string().c_str(), &icons[0].width, &icons[0].height, 0, SOIL_LOAD_RGBA);
			glfwSetWindowIcon(window, 1, icons);
			SOIL_free_image_data(icons[0].pixels);
		}

		[[maybe_unused]]
		auto path = GetExecutableFolder();

		// Main application loop
		while (!glfwWindowShouldClose(window))
		{
			// Perform event and input polling
			glfwPollEvents();

			// Load new font if necessary
			UpdateCustomFont();

			// Start the Dear ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			// Use custom font for this frame
			ImFont * font = s_customFont;
			if (font)
				ImGui::PushFont(font);

			// Clear render buffer
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			glClearColor(s_backgroundColor[0], s_backgroundColor[1], s_backgroundColor[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Perform app-specific updates
			s_appPtr->OnUpdate();

			// Pop custom font at the end of the frame
			if (font)
				ImGui::PopFont();

			// Render ImGui to draw data
			ImGui::Render();

			// Render ImGui
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			// Update and Render additional Platform Windows
			if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
			{
				GLFWwindow * backup_current_context = glfwGetCurrentContext();
				ImGui::UpdatePlatformWindows();
				ImGui::RenderPlatformWindowsDefault();
				glfwMakeContextCurrent(backup_current_context);
			}

			// Present buffer
			glfwSwapBuffers(window);
		}

		// Delete application
		s_appPtr = nullptr;

		// Save config data to disk
		SaveConfig(ini, orgName, appName);

		// Shut down ImGui and ImPlot
		ImGui_ImplOpenGL3_DestroyFontsTexture();
		ImGui_ImplGlfw_Shutdown();
		ImGui_ImplOpenGL3_Shutdown();
		ImPlot::DestroyContext();
		ImGui::DestroyContext();

		// Shut down glfw
		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
    }

}
