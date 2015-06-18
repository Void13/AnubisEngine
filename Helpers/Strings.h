#ifndef H__STRINGS
#define H__STRINGS

#include <vector>
#include <string>

namespace StringHelper
{
	std::vector<std::string> Split(std::string const &sSrc, std::string const &sDelimiter);
	std::string string_format(std::string const &fmt_str, ...);

	void ToLower(std::string &s);
	void ToUpper(std::string &s);

	std::string &LeftTrim(std::string &s, char _cChar = ' ');
	std::string &RightTrim(std::string &s, char _cChar = ' ');
	std::string &Trim(std::string &s, char _cChar = ' ');

	void deleteCRLF(std::string &str);
	void deleteCRLF2(std::wstring &str);

	void SplitFileName(const std::string &_str, const char *_szTok, std::string &path, std::string &filename);
	void SplitFileName(const std::wstring &_str, const wchar_t *_wszTok, std::wstring &path, std::wstring &filename);

	std::wstring s2ws(const std::string& str);
	std::string ws2s(const std::wstring& wstr);

	template <typename T>
	std::string NumberToString(T Number)
	{
		std::stringstream ss;
		ss << Number;
		return ss.str();
	}

	template <typename T>
	T StringToNumber(std::string const &Text)
	{
		std::stringstream ss(Text);
		T result;
		return (ss >> result) ? result : 0;
	}

	// templated version of my_equal so it could work with both char and wchar_t
	template<typename T>
	struct CUpperEqual
	{
		CUpperEqual(const std::locale &loc) : loc_(loc)
		{
		}

		bool operator()(T ch1, T ch2)
		{
			return std::toupper(ch1, loc_) == std::toupper(ch2, loc_);
		}
	private:
		const std::locale& loc_;
	};

	// find substring (case insensitive)
	template<typename T>
	int FindSubstrCI(const T& str1, const T& str2, const std::locale& loc = std::locale())
	{
		typename T::const_iterator it = 
			std::search(str1.begin(), str1.end(), str2.begin(), str2.end(), 
			CUpperEqual<typename T::value_type>(loc));

		if (it != str1.end()) 
			return it - str1.begin();
		
		return -1; // not found
	};
};

#endif