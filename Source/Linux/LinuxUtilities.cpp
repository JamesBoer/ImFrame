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


#include <unistd.h>
#include <pwd.h>
#include <limits.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#define GLFW_EXPOSE_NATIVE_X11
#include <GLFW/glfw3native.h>

#include "../ImfInternal.h"

namespace ImFrame
{

	std::filesystem::path OsGetConfigFolder()
	{
		char * rootDir = getenv("XDG_CONFIG_HOME");
		std::filesystem::path folder;
		if (!rootDir)
		{
			rootDir = getenv("HOME");
			if (!rootDir)
			{
				rootDir = getpwuid(getuid())->pw_dir;
				if (!rootDir)
					return std::filesystem::path();
			}
			folder = rootDir;
			folder += "/.config";
		}	
		return folder;
	}

	std::filesystem::path OsGetExecutableFolder()
	{
		char result[PATH_MAX];
		ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
		auto p = std::filesystem::path(result, (count > 0) ? count : 0);
		p.remove_filename();
		return p;
	}

	std::filesystem::path OsGetResourceFolder()
	{
		return OsGetExecutableFolder();
	}

	void * OsGetNativeWindow(GLFWwindow * window)
	{
		return reinterpret_cast<void *>(glfwGetX11Window(window));
	}

    void OsInitialize()
    {
    }

    void OsShutDown()
    {
    }

}
