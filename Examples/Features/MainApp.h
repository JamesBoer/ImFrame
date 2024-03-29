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

namespace Features
{
	class MainApp : public ImFrame::ImApp
	{
	public:
		MainApp(GLFWwindow * window);
		virtual ~MainApp();
		void OnUpdate() override;
		void OnKeyPress(int key, int mods) override;

	private:
		void Open();
		void SaveAs();
		void PickFolder();

        bool m_showExtraMenu = false;
        bool m_showHelpTopics = false;
        bool m_showGlDemo = false;
		bool m_showImGuiDemo = false;
		bool m_showImPlotDemo = false;
		bool m_setBgColor = false;
		bool m_setUiFont = false;
		ImFrame::TextureInfo m_texture;
		bool m_showTexture = false;
        bool m_showAbout = false;
	};
}
