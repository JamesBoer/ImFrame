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

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#undef APIENTRY

#include <linmath.h>
#include <imgui.h>
#include <implot.h>

#include <memory>
#include <string>
#include <functional>

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
		{ }
		virtual ~ImApp() {}

		virtual void OnKeyEvent([[maybe_unused]] int key, [[maybe_unused]] int scancode, [[maybe_unused]] int action, [[maybe_unused]] int mods) {}
		virtual void OnUpdate() {}

		GLFWwindow * GetWindow() const { return m_window; }

	private:
		GLFWwindow * m_window;
	};

	using ImAppPtr = std::unique_ptr<ImApp>;


	using ImAppCreateFn = std::function<ImAppPtr(GLFWwindow * window)>;

	// Call from main() to run ImFrame framework
    void RunImFrame(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn);

}