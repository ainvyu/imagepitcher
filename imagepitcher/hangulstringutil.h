#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

enum HanType { HAN_HEAD, HAN_MID, HAN_TAIL };

class CHangulStringUtil
{
public:
	static BOOL	IsHangul(const TCHAR &ch) { return (ch & 0x80) ? TRUE : FALSE; };		// ÇÑ±Û
	static BOOL IsHangul(const char& ch)  { return (ch & 0xE0) > 0x90; }

	static void    SplitHangul(const wchar_t& chCompleteHangul, wchar_t& chHead, wchar_t& chMid, wchar_t& chTail);
	static int     SplitHangulString(const wchar_t* pStr, wchar_t* pBuffer, UINT nSize);
	static wchar_t JoinHangul(wchar_t chHead, wchar_t chMid, wchar_t chTail);
	static void    JoinHangulString(const wchar_t* pBuffer, wchar_t* chHan);
	static int     GetIndex(HanType eType, wchar_t ch);
};

