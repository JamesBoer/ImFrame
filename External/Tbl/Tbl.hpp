/*
The MIT License (MIT)

Copyright (c) 2019 James Boer

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
#define TBL_WINDOWS
#define USE_FROM_CHARS
#pragma warning(push)
#pragma warning(disable : 4530) // Silence warnings if exceptions are disabled
#endif

#if defined(__linux__) || defined(__linux)
#define TBL_LINUX
#endif

#ifdef __APPLE__
#ifdef __MACH__
#define TBL_MACOS
#endif
#endif

#include <cassert>
#include <variant>
#include <string>
#include <string_view>
#include <vector>
#include <unordered_map>
#include <algorithm>
#ifdef USE_FROM_CHARS
#include <charconv>
#else
#include <sstream>
#endif

#ifdef TBL_WINDOWS
#pragma warning(pop)
#endif

namespace Tbl
{

	// Type indices for TableData variant
	const size_t IntType = 0;
	const size_t DoubleType = 1;
	const size_t StringType = 2;

	// Table class reads and parses CSV or tab-delimited text
	template<typename Alloc = std::allocator<char>>
	class Table
	{
	public:
		using String = std::basic_string<char, std::char_traits<char>, Alloc>;
		using TableData = std::variant<int64_t, double, String>;
	private:
		using TableDataAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<TableData>;
		using VectorTableData = std::vector<TableData, TableDataAlloc>;
		using StringIndexPair = std::pair<const String, size_t>;
		using StringIndexPairAlloc = typename std::allocator_traits<Alloc>::template rebind_alloc<StringIndexPair>;
		using StringIndexMap = std::unordered_map<const String, size_t, std::hash<String>, std::equal_to<String>, StringIndexPairAlloc>;
	public:

		Table(std::string_view text)
		{
			m_error = !Read(text);
		}

		explicit operator bool() const { return !m_error; }

		size_t GetNumColumns() const { return m_columnMap.size(); }
		size_t GetNumRows() const { return m_rowMap.size(); }
		size_t GetRowIndex(const String & rowName) const
		{
			const auto& pair = m_rowMap.find(rowName);
			assert(pair != m_rowMap.end());
			return pair->second;
		}
		size_t GetColumnIndex(const String & columnName) const
		{
			const auto& pair = m_columnMap.find(columnName);
			assert(pair != m_columnMap.end());
			return pair->second;
		}
		const TableData& GetData(size_t rowIndex, size_t columnIndex) const
		{
			assert(!m_error);
			assert(rowIndex < GetNumRows());
			assert(columnIndex < GetNumColumns());
			size_t index = columnIndex + (rowIndex * GetNumColumns());
			assert(index < m_tableData.size());
			return m_tableData[index];
		}
		const TableData& GetData(const String & rowName, const String & columnName) const
		{
			return GetData(GetRowIndex(rowName), GetColumnIndex(columnName));
		}
		template <typename T>
		const T & Get(size_t rowIndex, size_t columnIndex) const
		{
			return std::get<T>(GetData(rowIndex, columnIndex));
		}
		template <typename T>
		const T & Get(const String & rowName, const String & columnName) const
		{
			return std::get<T>(GetData(GetRowIndex(rowName), GetColumnIndex(columnName)));
		}

	private:

		enum class Format
		{
			International,
			Continental
		};

		bool IsLineEnd(std::string_view::const_iterator current) const
		{
			return *current == '\n' || *current == '\r';
		}

		void AdvanceToNextLine(std::string_view text, std::string_view::const_iterator & current) const
		{
			while (current != text.end())
			{
				if (!IsLineEnd(current))
					break;
				++current;
			}
		}

		bool DetectDelimiter(std::string_view text, char & delimiter, Format & format) const
		{
			size_t tabCount = 0;
			size_t commaCount = 0;
			size_t semicolonCount = 0;
			auto current = text.begin();
			while (current != text.end())
			{
				const char c = *current;
				if (c == ',')
					++commaCount;
				else if (c == '\t')
					++tabCount;
				else if (c == ';')
					++semicolonCount;
				else if (IsLineEnd(current))
					break;
				++current;
			}
			if (tabCount == 0 && commaCount == 0 && semicolonCount == 0)
				return false;
			delimiter = (commaCount >= tabCount) ? ((commaCount > semicolonCount) ? ',' : ';') : '\t';
			format = delimiter == ';' ? Format::Continental : Format::International;
			return true;
		}

        bool ParseInteger(const String & str, int64_t& intValue) const
        {
#ifdef USE_FROM_CHARS
            auto result = std::from_chars(str.data(), str.data() + str.size(), intValue);
            if (result.ptr == str.data() + str.size())
                return true;
            return false;
#else
            char * endPtr;
            intValue = strtoll(str.data(), &endPtr, 10);
            if (endPtr != (str.data() + str.size()))
                return false;
            return true;
#endif
        }

        bool ParseDouble(const String & str, double & doubleValue, Format format) const
        {
#ifdef USE_FROM_CHARS
			// In case contintental format is used, replace commas with decimal point
			if (format == Format::Continental)
			{
				String s = str;
				std::replace(s.begin(), s.end(), ',', '.');
				auto result = std::from_chars(s.data(), s.data() + s.size(), doubleValue);
				if (result.ptr == s.data() + s.size())
					return true;
			}
			else
			{
				auto result = std::from_chars(str.data(), str.data() + str.size(), doubleValue);
				if (result.ptr == str.data() + str.size())
					return true;
			}
            return false;
#else
			if (format == Format::Continental)
			{
				String s = str;
				std::replace(s.begin(), s.end(), ',', '.');
				std::istringstream istr(s.c_str());
				istr.imbue(std::locale::classic());
				istr >> doubleValue;
				if (istr.fail())
					return false;
			}
			else
			{
				std::istringstream istr(str.c_str());
				istr.imbue(std::locale::classic());
				istr >> doubleValue;
				if (istr.fail())
					return false;
			}
            return true;
#endif
        }

		TableData ParseData(const String & str, Format format) const
		{
			int64_t intValue = 0;
			if (ParseInteger(str, intValue))
				return intValue;
			else
			{
				double doubleValue = 0.0;
				if (ParseDouble(str, doubleValue, format))
					return doubleValue;
			}
			return str;
		}

		String ParseCell(std::string_view text, char delimiter, std::string_view::const_iterator & current) const
		{
			// Check if this cell is double-quoted
			bool quoted = *current == '"';
			if (quoted)
				++current;

			String str;
			while (current != text.end())
			{
				const char c = *current;
				if (quoted)
				{
					// Since this cell is double-quote delimited, proceed without checking delimiters until
					// we see another double quote character.
					if (c == '"')
					{
						// Advance the iterator and check to see if it's followed by the end of file or
						// delimiters.  If so, we're done parsing.  If not, a second double-quote should
						// follow.
						++current;
						if (current == text.end() || *current == delimiter || IsLineEnd(current))
							break;
						// If this assert hits, your data is malformed, since an interior double-quote was not
						// followed by a second quote
						assert(*current == '"');
					}
				}
				else
				{
					// This isn't a quote-escaped cell, so check for normal delimiters
					if (c == delimiter || IsLineEnd(current))
						break;
				}
				str += c;
				++current;
			}
			return str;
		}

		bool ReadHeader(std::string_view text, std::string_view::const_iterator & current, char delimiter)
		{
			while (current != text.end())
			{
				auto str = ParseCell(text, delimiter, current);
				m_columnMap.insert({ str, m_columnMap.size() });
				if (IsLineEnd(current))
					break;
				++current;
			}
			AdvanceToNextLine(text, current);
			return true;
		}

		bool ReadRow(std::string_view text, std::string_view::const_iterator & current, char delimiter, Format format)
		{
			// Track column data
			size_t column = 0;
			while (current != text.end())
			{
				auto str = ParseCell(text, delimiter, current);
				if (column == 0)
				{
					m_rowMap.insert({ str, m_rowMap.size() });
					m_tableData.push_back(str);
				}
				else
					m_tableData.push_back(ParseData(str, format));
				++column;
				if (current == text.end() || IsLineEnd(current))
					break;
				++current;
			}
			AdvanceToNextLine(text, current);
			return column == GetNumColumns();
		}

		bool ReadRows(std::string_view text, std::string_view::const_iterator & current, char delimiter, Format format)
		{
			while (current != text.end())
			{
				if (!ReadRow(text, current, delimiter, format))
					return false;
			}
			return true;
		}

		bool Read(std::string_view text)
		{
			char delimiter = 0;
			Format format = Format::International;
			if (!DetectDelimiter(text, delimiter, format))
				return false;
			std::string_view::const_iterator current = text.begin();
			if (!ReadHeader(text, current, delimiter))
				return false;
			if (!ReadRows(text, current, delimiter, format))
				return false;
			return true;
		}

		VectorTableData m_tableData;
		StringIndexMap m_columnMap;
		StringIndexMap m_rowMap;
		bool m_error = false;
	};

}


