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

#include "ImFrame.h"
#include "ImfUtilities.h"

#include <string>
#include <filesystem>
#include <cassert>

#include <Tbl.hpp>
#include <mini/ini.h>
#include <nfd.hpp>
#include <stb_image.h>

#include "ImGui/imgui_impl_glfw.h"
#include "ImGui/imgui_impl_opengl3.h"


namespace ImFrame
{
	std::filesystem::path OsGetConfigFolder();
	std::filesystem::path OsGetExecutableFolder();
	std::filesystem::path OsGetResourceFolder();
	void * OsGetNativeWindow(GLFWwindow * window);

    void OsInitialize();
    void OsShutDown();

#ifdef IMFRAME_MACOS
    bool OsBeginMainMenuBar();
    void OsEndMainMenuBar();
    bool OsBeginMenu(const char * label, bool enabled);
    void OsEndMenu();
    bool OsMenuItem(const char * label, const char * shortcut, bool selected, bool enabled);
    bool OsMenuItem(const char * label, const char * shortcut, bool * p_selected, bool enabled);
    void OsSeparator();
#endif

}
