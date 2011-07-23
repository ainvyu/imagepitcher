#pragma once

class CStringUtil
{
public:
	static BOOL	IsDigit(const TCHAR &ch)  { return _istdigit(ch) ? TRUE : FALSE; };		// 0-9
	static BOOL	IsAlpha(const TCHAR &ch)  { return _istalpha(ch) ? TRUE : FALSE; };		// 0-9, A-Z, a-z
	static BOOL	IsBlank(const TCHAR &ch)  { return ch == _T(' ') ? TRUE : FALSE; };			// ' '
	static BOOL	IsDigitAlpha(const TCHAR &ch) { return _istalnum(ch) ? TRUE : FALSE; }; // 0-9, A-Z, a-z
	static BOOL	IsDigitAlphaBlank(const TCHAR &ch) { return (_istalnum(ch) || IsBlank(ch)) ? TRUE : FALSE; };	// 0-9, A-Z, a-z, ' '
	static BOOL	IsLetter(const TCHAR &ch);

	static tstring MakeRadomString(int nLength);
  static std::string MakeRadomStringA(int nLength);

	// Split 은 Perl 스타일로 코딩하려고 Tokenize Redirect후 split된 vector를 반환.
	static std::vector<tstring> Split(const tstring& str, const tstring& separators = _T(" "));
  static std::vector<std::string> Split(const std::string& str, const std::string& separators = " ");
  static std::string Join(const std::vector<char>& vec, const std::string& delimiter = " ");
	static tstring Join(const std::vector<tstring>& vec, const tstring& delimiter = _T(" "));

	static void Tokenize(const std::string& str, std::list<std::string>& tokens, const std::string& delimiters = " ");
	static void Tokenize(const std::string& str, std::vector<std::string>& tokens, const std::string& delimiters = " ");
	static void Tokenize(const tstring& str, std::list<tstring>& tokens, const tstring& delimiters = _T(" "));
	static void Tokenize(const tstring& str, std::vector<tstring>& tokens, const tstring& delimiters = _T(" "));

	static tstring& Replace(tstring& source, const tstring& target, const tstring& replacement);
	static tstring& MakeLower(tstring& target);
	static tstring& MakeUpper(tstring& target);

	static tstring TrimRight(tstring& s, const tstring& drop );
	static tstring TrimLeft(tstring& s, const tstring& drop);
	static tstring Trim(tstring& s, const tstring& drop);

	static tstring LoadString( HINSTANCE hInstance, UINT uID );

	static std::string  Format(const char* fmt, ...);
	static std::wstring Format(const wchar_t* fmt, ...);

	static std::string IntToStrA(int nValue);
  static std::string Int64ToStrA( __int64 nValue );
	static tstring IntToStr(int nValue);
	static tstring IntToStr(int nValue, int radix);
	static tstring Int64ToStr(__int64 nValue);

	static int       StrToInt(const tstring& nValue);
	static int		 StrToInt( const std::string& nValue );
	static long long StrToInt64(const tstring& nValue);
	static long long StrToInt64( const std::string& nValue );

	static tstring VaListToString(LPCTSTR pstrFormat, va_list& argList);

	static bool IsTherePattern(const tstring& source, const tstring& pattern);

	static int CompareNoCase(const tstring& strTarget, const tstring& strSource);

	static int format_arg_list(std::string& strOut, const char* fmt, va_list args);
	static int format_arg_list(std::wstring& strOu, const wchar_t* fmt, va_list args);

    static void hex2str(char* h, char* s);
    static void str2hex(const char s[], char dest[], int n);
    

};
