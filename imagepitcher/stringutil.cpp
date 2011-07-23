#include "StdAfx.h"

#define _COMPILING_DLL
#include "StringUtil.h"
#include "HangulStringUtil.h"
#include "Util.h"

using namespace std;

tstring CStringUtil::Join(const vector<tstring>& vec, const tstring& delimiter)
{
  tstring ret_string;

  for (auto it = vec.begin(), e = vec.end(); it != e; ++it)
  {
    ret_string += *it;
    ret_string += delimiter;
  }

  // 맨 마지막에 추가되는 delimiter 는 제거
  if (!vec.empty() && !delimiter.empty())
    ret_string.erase(ret_string.size()-1, 1);

  return ret_string;
} 

std::string CStringUtil::Join(const std::vector<char>& vec, const std::string& delimiter)
{
    std::string ret_string;

    for (auto it = vec.begin(), e = vec.end(); it != e; ++it)
    {
        ret_string += *it;
        ret_string += delimiter;
    }

    // 맨 마지막에 추가되는 delimiter 는 제거
    if (!vec.empty() && !delimiter.empty())
        ret_string.erase(ret_string.size()-1, 1);

    return ret_string;
} 

void CStringUtil::Tokenize(const string& str, list<string>& tokens, const string& delimiters)
{
  string::size_type lastPos = str.find_first_not_of(delimiters, 0); 	 // 맨 첫 글자가 구분자인 경우 무시
  string::size_type pos     = str.find_first_of(delimiters, lastPos); // 구분자가 아닌 첫 글자를 찾는다

  while (string::npos != pos || string::npos != lastPos)
  {
    tokens.push_back(str.substr(lastPos, pos - lastPos));	// token을 찾았으니 list에 추가한다
    lastPos = str.find_first_not_of(delimiters, pos);		// 구분자를 뛰어넘는다. "not_of"에 주의하라
    pos     = str.find_first_of(delimiters, lastPos); 		// 다음 구분자가 아닌 글자를 찾는다
  }
}

void CStringUtil::Tokenize(const string& str, vector<string>& tokens, const string& delimiters)
{
  string::size_type lastPos = str.find_first_not_of(delimiters, 0); 	 // 맨 첫 글자가 구분자인 경우 무시
  string::size_type pos     = str.find_first_of(delimiters, lastPos); // 구분자가 아닌 첫 글자를 찾는다

  while (string::npos != pos || string::npos != lastPos)
  {
    tokens.push_back(str.substr(lastPos, pos - lastPos));	// token을 찾았으니 list에 추가한다
    lastPos = str.find_first_not_of(delimiters, pos);		// 구분자를 뛰어넘는다. "not_of"에 주의하라
    pos     = str.find_first_of(delimiters, lastPos); 		// 다음 구분자가 아닌 글자를 찾는다
  }
}

void CStringUtil::Tokenize(const tstring& str, list<tstring>& tokens, const tstring& delimiters)
{
  tstring::size_type lastPos = str.find_first_not_of(delimiters, 0); 	 // 맨 첫 글자가 구분자인 경우 무시
  tstring::size_type pos     = str.find_first_of(delimiters, lastPos); // 구분자가 아닌 첫 글자를 찾는다

  while (tstring::npos != pos || tstring::npos != lastPos)
  {
    tokens.push_back(str.substr(lastPos, pos - lastPos));	// token을 찾았으니 list에 추가한다
    lastPos = str.find_first_not_of(delimiters, pos);		// 구분자를 뛰어넘는다. "not_of"에 주의하라
    pos     = str.find_first_of(delimiters, lastPos); 		// 다음 구분자가 아닌 글자를 찾는다
  }
}

void CStringUtil::Tokenize(const tstring& str, vector<tstring>& tokens, const tstring& delimiters)
{
  tstring::size_type lastPos = str.find_first_not_of(delimiters, 0); 	 // 맨 첫 글자가 구분자인 경우 무시
  tstring::size_type pos     = str.find_first_of(delimiters, lastPos); // 구분자가 아닌 첫 글자를 찾는다

  while (tstring::npos != pos || tstring::npos != lastPos)
  {
    tokens.push_back(str.substr(lastPos, pos - lastPos));	// token을 찾았으니 list에 추가한다
    lastPos = str.find_first_not_of(delimiters, pos);		// 구분자를 뛰어넘는다. "not_of"에 주의하라
    pos     = str.find_first_of(delimiters, lastPos); 		// 다음 구분자가 아닌 글자를 찾는다
  }
}

vector<tstring> CStringUtil::Split(const tstring& str, const tstring& separators )
{
  vector<tstring> result;
  CStringUtil::Tokenize(str, result, separators);

  return result;
}

vector<string> CStringUtil::Split(const string& str, const string& separators )
{
  vector<string> result;
  CStringUtil::Tokenize(str, result, separators);

  return result;
}

// vector<tstring> CStringUtil::Split(const tstring& text, const tstring& separators )
// {
// 	size_t n = text.length();
// 	vector<tstring> result;
// 	size_t start, stop;
// 
// 	start = text.find_first_not_of(separators, 0); // 맨 첫 글자가 구분자인 경우 무시
// 	while ((start >= 0) && (start < n)) 
// 	{
// 		stop = text.find_first_of(separators, start);
// 		if ((stop < 0) || (stop > n))
// 			stop = n;
// 
// 		result.push_back(text.substr(start, stop - start));
// 		start = text.find_first_not_of(separators, stop+1);
// 	}
// 
// 	return result;
// }

bool CStringUtil::IsTherePattern(const tstring& source, const tstring& pattern)
{
  return source.find(pattern) != tstring::npos;
}

tstring& CStringUtil::Replace(tstring& source, const tstring& target, const tstring& replacement)
{
  tstring::size_type pos = 0;  // cur position
  tstring::size_type found = 0;   // found data position

  if (target.size () > 0)  // searching for nothing will cause a loop
  {
    while ((found = source.find (target, pos)) != tstring::npos)
    {
      source.replace (found, target.size(), replacement);
      pos = found + replacement.size();
    }
  }

  return source;
}

tstring& CStringUtil::MakeLower(tstring& target)
{
  std::transform(target.begin(), target.end(), target.begin(), _totlower);
  return target;
}

tstring& CStringUtil::MakeUpper(tstring& target)
{
  std::transform(target.begin(), target.end(), target.begin(), _totupper);
  return target;
}

tstring CStringUtil::TrimRight(tstring& s, const tstring& drop )
{
  tstring r = s.erase( s.find_last_not_of( drop) + 1 );
  return r;
}

tstring CStringUtil::TrimLeft(tstring& s, const tstring& drop)
{
  tstring l = s.erase(0, s.find_first_not_of( drop ) );
  return l;
}

tstring CStringUtil::Trim(tstring& s, const tstring& drop)
{
  return CStringUtil::TrimLeft(CStringUtil::TrimRight(s, drop), drop);
}

tstring CStringUtil::LoadString( HINSTANCE hInstance, UINT uID )
{
  tstring retval;

  const size_t BUF_INIT_SIZE = 10;

  size_t num = BUF_INIT_SIZE;

  std::vector<TCHAR> buf( num );
  int read_num = ::LoadString( hInstance, uID, &buf[0], num );

  if( 0 == read_num )
    return retval;

  while( read_num == num -1 )
  {
    num = num * 2;
    buf.resize( num );

    read_num = ::LoadString( hInstance, uID, &buf[0], num );
  }

  retval = &buf[0]; 

  return retval;
}

int CStringUtil::format_arg_list(string& strOut, char const* fmt, va_list args)
{
  if (!fmt)  
  {
    return -1;
  }

  int result = -1;
  int length = 512;

  char* buffer = 0;
  while (result == -1)
  {
    if (buffer)
      delete [] buffer;

    buffer = new char [length + 1];
    memset(buffer, 0, length + 1);

    result = _vsnprintf_s(buffer, length, _TRUNCATE, fmt, args);
    length *= 2;
  }

  strOut = buffer;
  delete [] buffer;

  return result;
}

int CStringUtil::format_arg_list(wstring& strOut, wchar_t const* fmt, va_list args)
{
  if (!fmt) 
  {
    return -1;
  }

  int result = -1;
  int length = 512;

  wchar_t* buffer = 0;
  while (result == -1)
  {
    if (buffer)
      delete [] buffer;

    buffer = new wchar_t[length + 1];
    memset(buffer, 0, length + 1);

    result = _vsnwprintf_s(buffer, length, _TRUNCATE, fmt, args);
    length *= 2;
  }

  strOut = buffer;
  delete [] buffer;

  return result;
}

std::string CStringUtil::Format(char const* fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  string s;
  int length = format_arg_list(s, fmt, args);
  va_end(args);

  return s;
}

wstring CStringUtil::Format(wchar_t const* fmt, ...)
{
  va_list args;

  va_start(args, fmt);
  wstring s;
  int nLength = format_arg_list(s, fmt, args);
  va_end(args);

  return s;
}

tstring CStringUtil::IntToStr( int nValue )
{
  TCHAR szValue[32];
  _itot_s(nValue, szValue, 32, 10);

  return tstring(szValue);
}

string CStringUtil::IntToStrA( int nValue )
{
  char szValue[32];
  _itoa_s(nValue, szValue, 32, 10);

  return string(szValue);
}

tstring CStringUtil::IntToStr(int nValue, int radix)
{
  if( 0 == nValue)
    return _T("0");

  //자리수 구하기
  int length = 0;
  int quotient = nValue;
  while( quotient )
  {
    ++length ;
    quotient = (quotient / radix);
  }

  if( 0 > nValue )
    ++length;	//- 표시 위치

  std::vector<TCHAR> buf( length+1 );	// for '\0'
  _itot_s( nValue, &buf[0], length+1, radix );
  tstring strRetVal = &buf[0];

  return strRetVal;
}

tstring CStringUtil::Int64ToStr( __int64 nValue )
{
  TCHAR szValue[64];
  _i64tot_s(nValue, szValue, 64, 10);

  return tstring(szValue);
}

string CStringUtil::Int64ToStrA( __int64 nValue )
{
  char szValue[64];
  _i64toa_s(nValue, szValue, 64, 10);

  return string(szValue);
}

int CStringUtil::StrToInt( tstring const& nValue )
{
  return _ttoi(nValue.c_str());
}

long long CStringUtil::StrToInt64( tstring const& nValue )
{
  return _ttoi64(nValue.c_str());
}

int CStringUtil::StrToInt( const string& nValue )
{
  return atoi(nValue.c_str());
}

long long CStringUtil::StrToInt64( const string& nValue )
{
  return _atoi64(nValue.c_str());
}

tstring CStringUtil::VaListToString(LPCTSTR pstrFormat, va_list& argList)
{
  TCHAR buf[10*1024] = {0}; // 10KB
  _vstprintf_s(buf, _countof(buf), pstrFormat, argList);

  return tstring(buf);
}

int CStringUtil::CompareNoCase( const tstring& strTarget, const tstring& strSource )
{
  return _tcsicmp(strTarget.c_str(), strSource.c_str());
}

BOOL CStringUtil::IsLetter( const TCHAR &ch )
{
  return (IsAlpha(ch) || CHangulStringUtil::IsHangul(ch)) ? TRUE : FALSE;				// 0-9, A-Z, a-z, 한글
}

tstring CStringUtil::MakeRadomString(int nLength)
{
  static unsigned int nCount = 0;
  srand((unsigned int)CUtil::GetTickCount() + ++nCount);
  tstring strRandom;

  static TCHAR candidate[3] = {'a', 'A', '0'};
  static int candidate_range[3] = {26, 26, 10};
  for (int i = 0; i < nLength; ++i)
  {
    int nCand = rand() % 3;
    strRandom += candidate[nCand] + rand() % candidate_range[nCand];
  }

  return strRandom;
}

std::string CStringUtil::MakeRadomStringA(int nLength)
{
  static unsigned int nCount = 0;
  srand((unsigned int)CUtil::GetTickCount() + ++nCount);
  std::string strRandom;

  static char candidate[3] = {'a', 'A', '0'};
  static int candidate_range[3] = {26, 26, 10};
  for (int i = 0; i < nLength; ++i)
  {
    int nCand = rand() % 3;
    strRandom += candidate[nCand] + rand() % candidate_range[nCand];
  }

  return strRandom;
}

// void CStringUtil::hex2str(char* h, char* s)
// {
//     char t[3] = {0, 0, 0};
// 
//     for (; h[0] && h[1]; h += 2)
//     {
//         t[0] = h[0]; 
//         t[1] = h[1];
//         *s++ = strtol(t, 0, 16); 
//     }
// 
//     *s = 0;
// }

void CStringUtil::hex2str(char* h, char* s)
{
    int i;
    while (sscanf(h, "%02x", &i) == 1) 
    {
        *s++ = i; 
        h += 2; 
    } 

    *s = 0; 
}

void CStringUtil::str2hex(const char s[], char dest[], int n)
{
    // 주어진 문자열을 헥사코드 형태로 출력한다.
    for (int i = 0; n >= 0; s++, i += 2) 
    { 
        sprintf(dest + i, "%0.2x", (unsigned char)*s); 
    } 
}