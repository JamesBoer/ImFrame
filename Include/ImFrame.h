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

#pragma once

// Platform definitions
#if defined(_WIN32) || defined(_WIN64)
#define IMFRAME_WINDOWS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif
#if defined(__linux__) || defined(__linux)
#define IMFRAME_LINUX
#endif
#if defined(__APPLE__) && defined(__MACH__)
#define IMFRAME_MACOS
#endif

// Enable memory leak detection in Windows debug builds
// Don't change the order of these includes!
#ifdef IMFRAME_WINDOWS
#define _CRTDBG_MAP_ALLOC
#endif

// Stdlib.h must come before crtdebug.h
#include <stdlib.h>

// Use the debug heap with leak reporting in Windows debug builds
#ifdef IMFRAME_WINDOWS
#include <crtdbg.h>
#endif

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#undef APIENTRY

#include <linmath.h>
#include <imgui.h>
#include <implot.h>

#include <memory>
#include <string>
#include <functional>
#include <optional>
#include <vector>
#include <filesystem>

namespace ImFrame
{
	/// Major version number
	const uint32_t MajorVersion = 0;

	/// Minor version number
	const uint32_t MinorVersion = 0;

	/// Patch number
	const uint32_t PatchNumber = 1;

	/// Get ImFrame version in string form for easier display
	std::string GetVersionString();

	/// Base application class
	class ImApp
	{
	public:
		ImApp(GLFWwindow * window) :
			m_window(window)
		{}
		virtual ~ImApp() {}

		// Input events
		virtual void OnKeyEvent([[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {}
		virtual void OnMouseButtonEvent([[maybe_unused]] int button, [[maybe_unused]] int action, [[maybe_unused]] int mods) {}
		virtual void OnCursorPosition([[maybe_unused]] double x, [[maybe_unused]] double y) {}

		// Window messages
		virtual void OnWindowPositionChange([[maybe_unused]] int x, [[maybe_unused]] int y) {}
		virtual void OnWindowSizeChange([[maybe_unused]] int width, [[maybe_unused]] int height) {}
		virtual void OnWindowMaximize([[maybe_unused]] bool maximized) {}

		// Update tick
		virtual void OnUpdate() {}

		GLFWwindow * GetWindow() const { return m_window; }

	private:
		GLFWwindow * m_window;
	};

	using ImAppPtr = std::unique_ptr<ImApp>;


	// Native file and folder dialog functions
	std::optional<std::filesystem::path> OpenFileDialog(const char * filters, const char * defaultPath);
	std::optional<std::vector<std::filesystem::path>> OpenFilesDialog(const char * filters, const char * defaultPath);
	std::optional<std::filesystem::path> SaveFileDialog(const char * filters, const char * defaultPath);
	std::optional<std::filesystem::path> PickFolderDialog(const char * defaultPath);

	// UI Fonts
	enum class FontType
	{
		CarlitoRegular,
		OpenSansRegular,
		OpenSansSemiBold,
		RobotoMedium,
		RobotoRegular,
	};

	bool IsCustomFontEnabled();
	void EnableCustomFont(bool enable);
	FontType GetCustomFontType();
	void SetCustomFontType(FontType font);
	float GetCustomFontSize();
	void SetCustomFontSize(float pixelSize);

	// Application function callback signature
	using ImAppCreateFn = std::function<ImAppPtr(GLFWwindow * window)>;

	// Call from main() to run ImFrame framework
    int RunImFrame(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn);

}
