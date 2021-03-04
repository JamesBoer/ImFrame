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

#include "Internal.h"

using namespace Features;

MainApp::MainApp(GLFWwindow * window) :
	ImFrame::ImApp(window)
{
	InitGlDemo();
}

void MainApp::OnKeyEvent(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
}

void MainApp::OnUpdate()
{
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("File"))
		{
			if (ImGui::MenuItem("Open...", nullptr, &m_fileOpen))
			{
				auto path = ImFrame::OpenFilesDialog("png,jpg", nullptr);
				m_fileOpen = false;
				if (path)
				{
					//printf("%s", path.value().c_str());
				}
			}
			if (ImGui::MenuItem("Save As...", nullptr, &m_fileSaveAs))
			{
				auto path = ImFrame::SaveFileDialog("png,jpg", "TestFile.jpg");
				m_fileSaveAs = false;
				if (path)
				{
					//printf("%s", path.value().c_str());
				}
			}
			if (ImGui::MenuItem("Pick Folder...", nullptr, &m_pickFolder))
			{
				auto path = ImFrame::PickFolderDialog(nullptr);
				m_pickFolder = false;
				if (path)
				{
					//printf("%s", path.value().c_str());
				}
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("View"))
		{
			ImGui::MenuItem("Show OpenGL Demo", nullptr, &m_showGlDemo);
			ImGui::MenuItem("Show ImGui Demo", nullptr, &m_showImGuiDemo);
			ImGui::MenuItem("Show ImPlot Demo", nullptr, &m_showImPlotDemo);
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	if (m_showGlDemo)
		ShowGlDemo(GetWindow());
	if (m_showImGuiDemo)
		ImGui::ShowDemoWindow(&m_showImGuiDemo);
	if (m_showImPlotDemo)
		ImPlot::ShowDemoWindow(&m_showImPlotDemo);
}
