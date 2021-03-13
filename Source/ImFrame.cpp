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

		// Register OnExit() as early as possible
		void OnExit();
		struct StaticInitializer
		{
			StaticInitializer()
			{
				// Signal this function to execute on exit
				atexit(OnExit);
			}
		};

		// Persistent app data

		struct PersistentData
		{
			// Window data
			int windowWidth = 800;
			int windowHeight = 600;
			int windowPosX = 100;
			int windowPosY = 100;
			bool windowMaximized = false;
			std::array<float, 3> backgroundColor = { 0.08f, 0.08f, 0.08f };

			// ImGui settings
			bool fontEnabled = true;
			FontType fontType = FontType::RobotoRegular;
			float fontSize = 15.0f;

			// Settings data for internal ImFrame data
			mINI::INIStructure imframeIni;

			// Settings data for application data
			mINI::INIStructure appIni;

			ImAppPtr appPtr;

			bool fontChanged = true;
			ImFont * customFont = nullptr;
		};


		StaticInitializer s_initializer;
		std::unique_ptr<PersistentData> s_data;


		void ErrorCallback([[maybe_unused]] int error, const char * description)
		{
			fprintf(stderr, "Error: %s\n", description);
		}

		void KeyCallback([[maybe_unused]] GLFWwindow * window, int key, int scancode, int action, int mods)
		{
			if (s_data->appPtr)
				s_data->appPtr->OnKeyEvent(key, scancode, action, mods);
		}

		void WindowPosCallback(GLFWwindow * window, int x, int y)
		{		
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				s_data->windowPosX = x;
				s_data->windowPosY = y;
			}
			if (s_data->appPtr)
				s_data->appPtr->OnWindowPositionChange(x, y);
		}

		void WindowSizeCallback(GLFWwindow * window, int width, int height)
		{
			if (!glfwGetWindowAttrib(window, GLFW_MAXIMIZED))
			{
				s_data->windowWidth = width;
				s_data->windowHeight = height;
			}
			if (s_data->appPtr)
				s_data->appPtr->OnWindowSizeChange(width, height);
		}

		void WindowMaximizeCallback([[maybe_unused]] GLFWwindow * window, int maximized)
		{
			s_data->windowMaximized = maximized ? true : false;
			if (s_data->appPtr)
				s_data->appPtr->OnWindowMaximize(s_data->windowMaximized);
		}

		void WindowMouseButtonCallback([[maybe_unused]] GLFWwindow * window, int button, int action, int mods)
		{
			if (s_data->appPtr)
				s_data->appPtr->OnMouseButtonEvent(button, action, mods);
		}

		void WindowCursorPositionCallback([[maybe_unused]] GLFWwindow * window, double x, double y)
		{
			if (s_data->appPtr)
				s_data->appPtr->OnCursorPosition(x, y);
		}

		std::string GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, const std::string & defaultValue)
		{
			const auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return s;
		}

		float GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, float defaultValue)
		{
			const auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stof(s);
		}

		double GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, double defaultValue)
		{
			const auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stod(s);
		}

		int GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, int defaultValue)
		{
			const auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stoi(s);
		}

		bool GetConfigValue(mINI::INIStructure & ini, const char * sectionName, const char * valueName, bool defaultValue)
		{
			const auto & s = ini[sectionName][valueName];
			if (s.empty())
				return defaultValue;
			return std::stoi(s) == 0 ? false : true;
		}

		void GetConfig(mINI::INIStructure & ini, const std::string & fileName, const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append(fileName);
			mINI::INIFile file(configFolder.string());
			file.read(ini);
		}

		void GetImFrameConfig(mINI::INIStructure & ini, const std::string & orgName, const std::string & appName)
		{
			GetConfig(ini, "imframe.ini", orgName, appName);
			s_data->windowWidth = GetConfigValue(ini, "window", "width", s_data->windowWidth);
			s_data->windowHeight = GetConfigValue(ini, "window", "height", s_data->windowHeight);
			s_data->windowPosX = GetConfigValue(ini, "window", "posx", s_data->windowPosX);
			s_data->windowPosY = GetConfigValue(ini, "window", "posy", s_data->windowPosY);
			s_data->windowMaximized = GetConfigValue(ini, "window", "maximized", s_data->windowMaximized);
			s_data->backgroundColor[0] = GetConfigValue(ini, "window", "bgcolorr", s_data->backgroundColor[0]);
			s_data->backgroundColor[1] = GetConfigValue(ini, "window", "bgcolorg", s_data->backgroundColor[1]);
			s_data->backgroundColor[2] = GetConfigValue(ini, "window", "bgcolorb", s_data->backgroundColor[2]);
			s_data->fontEnabled = GetConfigValue(ini, "font", "enabled", s_data->fontEnabled);
			s_data->fontType = static_cast<ImFrame::FontType>(GetConfigValue(ini, "font", "type", static_cast<int>(s_data->fontType)));
			s_data->fontSize = GetConfigValue(ini, "font", "size", s_data->fontSize);
		}

		void SaveConfig(mINI::INIStructure & ini, const std::string & fileName, const std::string & orgName, const std::string & appName)
		{
			namespace fs = std::filesystem;
			fs::path configFolder = GetConfigFolder(orgName, appName);
			configFolder.append(fileName);
			mINI::INIFile file(configFolder.string());
			file.write(ini);
		}

		void SaveImFrameConfig(mINI::INIStructure & ini, const std::string & orgName, const std::string & appName)
		{
			ini["window"]["width"] = std::to_string(s_data->windowWidth);
			ini["window"]["height"] = std::to_string(s_data->windowHeight);
			ini["window"]["posx"] = std::to_string(s_data->windowPosX);
			ini["window"]["posy"] = std::to_string(s_data->windowPosY);
			ini["window"]["maximized"] = std::to_string(s_data->windowMaximized ? 1 : 0);
			ini["window"]["bgcolorr"] = std::to_string(s_data->backgroundColor[0]);
			ini["window"]["bgcolorg"] = std::to_string(s_data->backgroundColor[1]);
			ini["window"]["bgcolorb"] = std::to_string(s_data->backgroundColor[2]);
			ini["font"]["enabled"] = std::to_string(s_data->fontEnabled ? 1 : 0);
			ini["font"]["type"] = std::to_string(static_cast<int>(s_data->fontType));
			ini["font"]["size"] = std::to_string(s_data->fontSize);
			SaveConfig(ini, "imframe.ini", orgName, appName);
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
			if (!s_data->fontChanged)
				return;
			if (s_data->fontEnabled)
			{
				ImGuiIO & io = ImGui::GetIO();
				if (s_data->customFont)
					io.Fonts->Clear();
				switch (s_data->fontType)
				{
					case FontType::CarlitoRegular:
						s_data->customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&CarlitoRegular_compressed_data[0]), CarlitoRegular_compressed_size, s_data->fontSize);
						break;
					case FontType::OpenSansRegular:
						s_data->customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&OpenSansRegular_compressed_data[0]), OpenSansRegular_compressed_size, s_data->fontSize);
						break;
					case FontType::OpenSansSemiBold:
						s_data->customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&OpenSansSemiBold_compressed_data[0]), OpenSansSemiBold_compressed_size, s_data->fontSize);
						break;
					case FontType::RobotoMedium:
						s_data->customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&RobotoMedium_compressed_data[0]), RobotoMedium_compressed_size, s_data->fontSize);
						break;
					case FontType::RobotoRegular:
						s_data->customFont = io.Fonts->AddFontFromMemoryCompressedTTF((void *)(&RobotoRegular_compressed_data[0]), RobotoRegular_compressed_size, s_data->fontSize);
						break;
				}
				ImGui_ImplOpenGL3_CreateFontsTexture();
			}
			else
			{
				s_data->customFont = nullptr;
			}
			s_data->fontChanged = false;
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

    void SetBackgroundColor(std::array<float, 3> color)
    {
        s_data->backgroundColor = color;
    }

    std::array<float, 3> GetBackgroundColor()
    {
        return s_data->backgroundColor;
    }

	bool IsCustomFontEnabled()
	{
		return s_data->fontEnabled;
	}

	void EnableCustomFont(bool enable)
	{
		if (enable != s_data->fontEnabled)
		{
			s_data->fontEnabled = enable;
			s_data->fontChanged = true;
		}
	}

	FontType GetCustomFontType()
	{
		return s_data->fontType;
	}

	void SetCustomFontType(FontType font)
	{
		if (font != s_data->fontType)
		{
			s_data->fontType = font;
			s_data->fontChanged = true;
		}
	}

	float GetCustomFontSize()
	{
		return s_data->fontSize;
	}

	void SetCustomFontSize(float pixelSize)
	{
		if (pixelSize != s_data->fontSize)
		{
			s_data->fontSize = pixelSize;
			s_data->fontChanged = true;
		}
	}

	std::string GetConfigValue(const char * sectionName, const char * valueName, const std::string & defaultValue)
	{
		return GetConfigValue(s_data->appIni, sectionName, valueName, defaultValue);
	}

	float GetConfigValue(const char * sectionName, const char * valueName, float defaultValue)
	{
		return GetConfigValue(s_data->appIni, sectionName, valueName, defaultValue);
	}

	double GetConfigValue(const char * sectionName, const char * valueName, double defaultValue)
	{
		return GetConfigValue(s_data->appIni, sectionName, valueName, defaultValue);
	}

	int GetConfigValue(const char * sectionName, const char * valueName, int defaultValue)
	{
		return GetConfigValue(s_data->appIni, sectionName, valueName, defaultValue);
	}

	bool GetConfigValue(const char * sectionName, const char * valueName, bool defaultValue)
	{
		return GetConfigValue(s_data->appIni, sectionName, valueName, defaultValue);
	}

	void SetConfigValue(const char * sectionName, const char * valueName, const std::string & value)
	{
		s_data->appIni[sectionName][valueName] = value;
	}

	void SetConfigValue(const char * sectionName, const char * valueName, float value)
	{
		s_data->appIni[sectionName][valueName] = std::to_string(value);
	}

	void SetConfigValue(const char * sectionName, const char * valueName, double value)
	{
		s_data->appIni[sectionName][valueName] = std::to_string(value);
	}

	void SetConfigValue(const char * sectionName, const char * valueName, int value)
	{
		s_data->appIni[sectionName][valueName] = std::to_string(value);
	}

	void SetConfigValue(const char * sectionName, const char * valueName, bool value)
	{
		s_data->appIni[sectionName][valueName] = value ? "1" : "0";
	}


    bool BeginMainMenuBar()
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        return OsBeginMainMenuBar();
#else
        return ImGui::BeginMainMenuBar();
#endif
    }

    void EndMainMenuBar()
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        OsEndMainMenuBar();
#else
        ImGui::EndMainMenuBar();
#endif
    }

    bool BeginMenu(const char * label, bool enabled)
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        return OsBeginMenu(label, enabled);
#else
        return ImGui::BeginMenu(label, enabled);
#endif
    }

    void EndMenu()
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        OsEndMenu();
#else
        ImGui::EndMenu();
#endif
    }

    bool MenuItem(const char * label, const char * shortcut, bool selected, bool enabled)
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        return OsMenuItem(label, shortcut, selected, enabled);
#else
        return ImGui::MenuItem(label, shortcut, selected, enabled);
#endif
    }

    bool MenuItem(const char * label, const char * shortcut, bool * p_selected, bool enabled)
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        return OsMenuItem(label, shortcut, p_selected, enabled);
#else
        return ImGui::MenuItem(label, shortcut, p_selected, enabled);
#endif
    }

    void Separator()
    {
#if defined(IMFRAME_MACOS) && defined(IMFRAME_MACOS_MENUS)
        OsSeparator();
#else
        ImGui::Separator();
#endif
    }

    int Run(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn)
    {
		namespace fs = std::filesystem;

		// Allocate all persistent internal window/app data
		s_data = std::make_unique<PersistentData>();

#ifdef IMFRAME_WINDOWS
		// Enable memory leak checking
		_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
		_CrtSetReportMode(_CRT_ERROR, _CRTDBG_MODE_DEBUG);
#endif

		// Read existing config data
		GetImFrameConfig(s_data->imframeIni, orgName, appName);
		GetConfig(s_data->appIni, "app.ini", orgName, appName);

		// Init GLFW and create window
		glfwSetErrorCallback(ErrorCallback);
		if (!glfwInit())
		{
			return 1;
		}
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
		GLFWwindow * window = glfwCreateWindow(s_data->windowWidth, s_data->windowHeight, appName.c_str(), NULL, NULL);
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
		glfwSetWindowPos(window, s_data->windowPosX, s_data->windowPosY);
		if (s_data->windowMaximized)
			glfwMaximizeWindow(window);
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1);

		// Initialize glad GL functions
		gladLoadGL();

        // Initialize any OS-specific functionality
        OsInitialize();

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
		s_data->appPtr = createAppFn(window);

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
			ImFont * font = s_data->customFont;
			if (font)
				ImGui::PushFont(font);

			// Clear render buffer
			int width, height;
			glfwGetFramebufferSize(window, &width, &height);
			glViewport(0, 0, width, height);
			glClearColor(s_data->backgroundColor[0], s_data->backgroundColor[1], s_data->backgroundColor[2], 1.0f);
			glClear(GL_COLOR_BUFFER_BIT);

			// Perform app-specific updates
			s_data->appPtr->OnUpdate();

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
        
        // OS-specific shutdown
        OsShutDown();

		// Delete application
		s_data->appPtr = nullptr;

		// Save config data to disk
		SaveConfig(s_data->appIni, "app.ini", orgName, appName);
		SaveImFrameConfig(s_data->imframeIni, orgName, appName);

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
