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
	m_showExtraMenu = ImFrame::GetConfigValue("show", "extramenu", m_showExtraMenu);
	m_showGlDemo = ImFrame::GetConfigValue("show", "gldemo", m_showGlDemo);
	m_showImGuiDemo = ImFrame::GetConfigValue("show", "imguidemo", m_showImGuiDemo);
	m_showImPlotDemo = ImFrame::GetConfigValue("show", "implotdemo", m_showImPlotDemo);
}

MainApp::~MainApp()
{
	ImFrame::SetConfigValue("show", "extramenu", m_showExtraMenu);
	ImFrame::SetConfigValue("show", "gldemo", m_showGlDemo);
	ImFrame::SetConfigValue("show", "imguidemo", m_showImGuiDemo);
	ImFrame::SetConfigValue("show", "implotdemo", m_showImPlotDemo);
	if (m_texture.textureID)
		glDeleteTextures(1, &m_texture.textureID);

}

void MainApp::OnKeyEvent(int key, [[maybe_unused]] int scancode, int action, [[maybe_unused]] int mods)
{
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
}

void MainApp::OnUpdate()
{
	if (ImFrame::BeginMainMenuBar())
	{
		if (ImFrame::BeginMenu("File"))
		{
			if (ImFrame::MenuItem("Open...##Test", "O"))
			{
				auto path = ImFrame::OpenFileDialog({ {"Image files", "png,jpg" } }, nullptr);
				if (path)
				{
					auto ret = ImFrame::LoadTextureFromFile(path.value().string().c_str());
					if (ret)
					{
						if (m_texture.textureID)
							glDeleteTextures(1, &m_texture.textureID);
						m_texture = ret.value();
						m_showTexture = true;
					}
				}
			}
			if (ImFrame::MenuItem("Save As...", "S"))
			{
				auto path = ImFrame::SaveFileDialog({ {"Image files", "png,jpg" } }, nullptr, "TestFile.jpg");
				if (path)
				{
				}
			}
            ImFrame::Separator();
			if (ImFrame::MenuItem("Pick Folder...", nullptr))
			{
				auto path = ImFrame::PickFolderDialog(nullptr);
				if (path)
				{
				}
			}
#ifndef IMFRAME_MACOS
			ImFrame::Separator();
			if (ImFrame::MenuItem("Exit", nullptr))
			{
				glfwSetWindowShouldClose(GetWindow(), GLFW_TRUE);
			}
#endif
            ImFrame::EndMenu();
		}
		if (ImFrame::BeginMenu("View"))
		{
            ImFrame::MenuItem("Show Extra Menu", nullptr, &m_showExtraMenu);
            ImFrame::MenuItem("Show OpenGL Demo", nullptr, &m_showGlDemo);
            ImFrame::MenuItem("Show ImGui Demo", nullptr, &m_showImGuiDemo);
            ImFrame::MenuItem("Show ImPlot Demo", nullptr, &m_showImPlotDemo);
            ImFrame::Separator();
            ImFrame::MenuItem("Set Background Color...", nullptr, &m_setBgColor);
            ImFrame::MenuItem("Set UI Font...", nullptr, &m_setUiFont);
            ImFrame::EndMenu();
		}
        if (m_showExtraMenu && ImFrame::BeginMenu("Extra"))
        {
            ImFrame::MenuItem("Something extra!", nullptr);
            if (ImFrame::BeginMenu("Sub menu"))
            {
                ImFrame::MenuItem("One", nullptr);
                ImFrame::MenuItem("Two", nullptr);
                ImFrame::MenuItem("Three", nullptr);
                ImFrame::EndMenu();
            }
            ImFrame::EndMenu();
        }
        ImFrame::EndMainMenuBar();
	}

	if (m_showGlDemo)
		ShowGlDemo(GetWindow());
	if (m_showImGuiDemo)
		ImGui::ShowDemoWindow(&m_showImGuiDemo);
	if (m_showImPlotDemo)
		ImPlot::ShowDemoWindow(&m_showImPlotDemo);
	if (m_setBgColor)
		SetBgColor(&m_setBgColor);
	if (m_setUiFont)
		SetUiFont(&m_setUiFont);
	if (m_showTexture)
	{
		if (ImGui::Begin("OpenGL Texture Text", &m_showTexture, ImGuiWindowFlags_HorizontalScrollbar))
		{
			ImGui::Text("pointer = %p", reinterpret_cast<void *>(m_texture.textureID));
			ImGui::Text("size = %d x %d", m_texture.width, m_texture.height);
			ImGui::Image((void *)(intptr_t)m_texture.textureID, ImVec2((float)m_texture.width, (float)m_texture.height));
		}
		ImGui::End();
	}
}
