#include "EnginePCH.h"

#include "Strings.h"

namespace StringHelper
{
	std::vector<std::string> Split(std::string const &sSrc, std::string const &sDelimiter)
	{
		std::vector<std::string> Strings;

		std::string::size_type Pos = 0;
		std::string::size_type LastPos = 0;

		while (true)
		{
			Pos = sSrc.find(sDelimiter, LastPos);

			if (Pos != std::string::npos)
			{
				Strings.push_back(sSrc.substr(LastPos, Pos - LastPos));
			}
			else
			{
				break;
			}

			LastPos = Pos + sDelimiter.size();
		}

		if (LastPos < sSrc.size())
		{
			Strings.push_back(sSrc.substr(LastPos, sSrc.size() - LastPos));
		}

		return Strings;
	}

	std::string string_format(std::string const &fmt_str, ...)
	{
		int final_n = 0;
		int n = ((int)fmt_str.size()) * 2; /* Reserve two times as much as the length of the fmt_str */

		std::string str;
		std::unique_ptr<char[]> formatted;
		va_list ap;

		while (1)
		{
			formatted.reset(new char[n]); /* Wrap the plain char array into the unique_ptr */
			strcpy(&formatted[0], fmt_str.c_str());

			va_start(ap, fmt_str);
			final_n = vsnprintf(&formatted[0], n, fmt_str.c_str(), ap);
			va_end(ap);

			if (final_n < 0 || final_n >= n)
				n += abs(final_n - n + 1);
			else
				break;
		}

		return std::string(formatted.get());
	}

	void ToLower(std::string &s)
	{
		for (char &c : s)
		{
			c = tolower(c);
		}
	}

	void ToUpper(std::string &s)
	{
		for (char &c : s)
		{
			c = toupper(c);
		}
	}

	std::string &LeftTrim(std::string &s, char _cChar)
	{
		auto ItPos = s.begin();
		for (; ItPos != s.end(); ItPos++)
		{
			if (*ItPos != _cChar)
				break;
		}

		s.erase(s.begin(), ItPos);
		return s;
	}

	std::string &RightTrim(std::string &s, char _cChar)
	{
		auto ItPos = s.rbegin();
		for (; ItPos != s.rend(); ItPos++)
		{
			if (*ItPos != _cChar)
				break;
		}

		s.erase(ItPos.base(), s.end());
		return s;
	}

	std::string &Trim(std::string &s, char _cChar)
	{
		return LeftTrim(RightTrim(s, _cChar));
	}

	void deleteCRLF(std::string &str)
	{
		// delete 13 & 10

		std::vector<unsigned int> IDsToDelete;

		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (str[i] == '\r' || str[i] == '\n' || str[i] == '\t')
				IDsToDelete.push_back(i);
		}

		for (auto it = IDsToDelete.rbegin(); it != IDsToDelete.rend(); it++)
		{
			str.erase(str.begin() + *it);
		}
	}

	void deleteCRLF2(std::wstring &str)
	{
		// delete 13 & 10

		std::vector<unsigned int> IDsToDelete;

		for (unsigned int i = 0; i < str.size(); i++)
		{
			if (str[i] == '\r' || str[i] == '\n')
				IDsToDelete.push_back(i);
		}

		for (auto it = IDsToDelete.rbegin(); it != IDsToDelete.rend(); it++)
		{
			str.erase(str.begin() + *it);
		}
	}

	void SplitFileName(const std::string &_str, const char *_szTok, std::string &path, std::string &filename)
	{
		size_t found;
		found = _str.find_last_of(_szTok);
		path = _str.substr(0, found);
		filename = _str.substr(found + 1);
	}

	void SplitFileName(const std::wstring &_str, const wchar_t *_wszTok, std::wstring &path, std::wstring &filename)
	{
		size_t found;
		found = _str.find_last_of(_wszTok);
		path = _str.substr(0, found);
		filename = _str.substr(found + 1);
	}

	std::wstring s2ws(const std::string& str)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.from_bytes(str);
	}

	std::string ws2s(const std::wstring& wstr)
	{
		typedef std::codecvt_utf8<wchar_t> convert_typeX;
		std::wstring_convert<convert_typeX, wchar_t> converterX;

		return converterX.to_bytes(wstr);
	}
};