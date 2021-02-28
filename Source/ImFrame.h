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
#ifdef __APPLE__
#ifdef __MACH__
#define IMFRAME_MACOS
#endif
#endif

namespace ImFrame
{
	class ImApp
	{
	public:
		virtual ~ImApp() {}
		virtual void OnUpdate() = 0;

	private:

	};

	using ImAppPtr = std::unique_ptr<ImApp>;

	using ImAppCreateFn = std::function<ImAppPtr(void)>;

    void RunImFrame(const std::string & orgName, const std::string & appName, ImAppCreateFn createAppFn);

}
