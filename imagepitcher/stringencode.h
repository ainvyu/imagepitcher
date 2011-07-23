#pragma once

#ifdef _COMPILING_DLL
#define LIBSPEC __declspec(dllexport)
#else
#define LIBSPEC __declspec(dllimport)
#endif

#ifdef UNICODE
#define CONVERT_LOCAL     CStringEncode::utf8_to_widechar
#define CONVERT_UTF8	  CStringEncode::widechar_to_utf8
#define CONVERT_WIDE      CStringEncode::mbs_to_wcs
#define CONVERT_MULTIBYTE CStringEncode::wcs_to_mbs
#else
#define CONVERT_LOCAL     CStringEncode::utf8_to_multibyte
#define CONVERT_UTF8      CStringEncode::multibyte_to_utf8
#define CONVERT_WIDE      CStringEncode::local_to_local
#define CONVERT_MULTIBYTE CStringEncode::local_to_local
#endif // UNICODE

// 사용 전에 반드시 InitLocale()를 부른후 사용.
class CStringEncode
{
public:
  static void InitLocale();

  static std::wstring mbs_to_wcs(std::string const& str, std::locale const& loc = std::locale());
  static std::string  wcs_to_mbs(std::wstring const& str, std::locale const& loc = std::locale());
  static std::wstring mbs_to_wcs2(std::string const& str, UINT codePage = CP_ACP); 
  static std::string  wcs_to_mbs2(std::wstring const& str, UINT codePage = CP_ACP);
  static std::wstring mbs_to_wcs3(std::string const& str); 
  static std::string  wcs_to_mbs3(std::wstring const& str);

    static std::u16string utf8_to_utf16(std::string const& str, std::locale const& loc = std::locale());
    static std::string    utf16_to_utf8(std::u16string const& str, std::locale const& loc = std::locale());
    static std::u32string utf8_to_utf32(std::string const& str, std::locale const& loc = std::locale());
    static std::string    utf32_to_utf8(std::u32string const& str, std::locale const& loc = std::locale());

  static tstring		local_to_local(const tstring& str);
  static std::string	multibyte_to_utf8(std::string const& str);
  static std::string	utf8_to_multibyte(std::string const& str);
  static std::wstring utf8_to_widechar(std::string const& str);
  static std::string  widechar_to_utf8(std::wstring const& str);

  // Native (특별한 경우에 쓴다. 이건 STL 안쓰고 직접 계산해서 인코딩한다. 그외엔 위에꺼 쓰면 됨.
  static int UTF8ToUnicode( LPBYTE src, LPWSTR dst);
  static int UnicodeToUTF8( LPCWSTR src, LPBYTE dst);
  static int GetUTF8LengthFromUTF16String(const wchar_t* szText, unsigned int nLen);
  static int GetUTF8CharLengthFromUTF16(wchar_t ch);
    

  // 검토 안된거.
  static void UTF8FromUCS2(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len);
  static unsigned int UCS2Length(const char *s, unsigned int len);
  static unsigned int UCS2FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen);

  static void UTF8FromUTF16(const wchar_t *uptr, unsigned int tlen, char *putf, unsigned int len);
    static unsigned int UTF16FromUTF8(const char *s, unsigned int len, wchar_t *tbuf, unsigned int tlen);
  static unsigned int GetUTF16LengthFromUTF8String(const char *s, unsigned int len);

    // Native (이건 STL 쓴거)
    static std::string UTF8FromUTF16(const std::wstring& strUTF16);
    static std::wstring UTF16FromUTF8(const std::string& strUTF8);
    

private:
  static bool m_bInitLocale;
};

