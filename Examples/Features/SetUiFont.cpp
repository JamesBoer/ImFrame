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

namespace Features
{

	void SetUiFont(bool * show)
	{
		// Specify initial layout if not read from ini
		const ImGuiViewport * main_viewport = ImGui::GetMainViewport();
		ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x + 50, main_viewport->WorkPos.y + 50), ImGuiCond_FirstUseEver);
		ImGui::SetNextWindowSize(ImVec2(350, 250), ImGuiCond_FirstUseEver);

		// Main body of the set ui font window starts here.
		if (!ImGui::Begin("Set UI Font", show))
		{
			ImGui::End();
			return;
		}

		bool enableCustomFont = ImFrame::IsCustomFontEnabled();
		if (ImGui::Checkbox("Enable Custom UI Font", &enableCustomFont))
			ImFrame::EnableCustomFont(enableCustomFont);

		if (enableCustomFont)
		{
			const char * fonts[] = { "Carlito Regular", "OpenSans Regular", "OpenSans Semi-Bold", "Roboto Medium", "Roboto Regular" };
			auto currentFont = ImFrame::GetCustomFont();
			int index = static_cast<int>(currentFont.first);
			if (ImGui::Combo("Font Name", &index, fonts, IM_ARRAYSIZE(fonts)))
			{
				currentFont.first = static_cast<ImFrame::UiFont>(index);
				ImFrame::SetCustomFont(currentFont.first, currentFont.second);
			}
			ImU8 size = static_cast<ImU8>(currentFont.second);
			const ImU8 one = 1;
			if (ImGui::InputScalar("input u8", ImGuiDataType_U8, &size, &one, NULL, "%u"))
			{
				size = std::clamp<ImU8>(size, 10, 24);
				currentFont.second = static_cast<float>(size);
				ImFrame::SetCustomFont(currentFont.first, currentFont.second);
			}
		}
		ImGui::End();
	}

}
