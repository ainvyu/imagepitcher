#include "StdAfx.h"

#define _COMPILING_DLL
#include "StringEncode.h"
#include "Util.h"

#include "utf8_codecvt_facet.hpp"

enum { 
    SURROGATE_LEAD_FIRST = 0xD800,
    SURROGATE_TRAIL_FIRST = 0xDC00,
    SURROGATE_TRAIL_LAST = 0xDFFF,
};

bool CStringEncode::m_bInitLocale = false;

void CStringEncode::InitLocale()
{
  std::locale::global(std::locale(""));
  CStringEncode::m_bInitLocale = true;
}

tstring CStringEncode::local_to_local(const tstring& str)
{
    return str;
}

std::string CStringEncode::multibyte_to_utf8(std::string const& str)
{
    // new로 생성된 utf8_codecvt를 delete할 필요는 없다. 
    // locale 내부에서 알아서 delete 해준다.
    std::locale loc(std::locale(""), new utf8_codecvt_facet);

    return wcs_to_mbs(mbs_to_wcs(str), loc);
}

std::string CStringEncode::utf8_to_multibyte(std::string const& str)
{
    // new로 생성된 utf8_codecvt를 delete할 필요는 없다. 
    // locale 내부에서 알아서 delete 해준다.
    std::locale loc(std::locale(""), new utf8_codecvt_facet);

    return wcs_to_mbs(mbs_to_wcs(str, loc));
}

// utf8 -> wstring
// std::wstring CStringEncode::utf8_to_widechar(std::string const& str)
// {
//     // new로 생성된 utf8_codecvt를 delete할 필요는 없다. 
//     // locale 내부에서 알아서 delete 해준다.
//     std::locale loc(std::locale(""), new utf8_codecvt_facet);
// 
//     return mbs_to_wcs(str, loc);
// }
std::wstring CStringEncode::utf8_to_widechar(std::string const& str)
{
    return UTF16FromUTF8(str);
}

// wstring -> utf8
// std::string CStringEncode::widechar_to_utf8(std::wstring const& str)
// {
//     // new로 생성된 utf8_codecvt를 delete할 필요는 없다. 
//     // locale 내부에서 알아서 delete 해준다.
//     std::locale loc(std::locale(""), new utf8_codecvt_facet);
// 
//     return wcs_to_mbs(str, loc);
// }
std::string CStringEncode::widechar_to_utf8(std::wstring const& str)
{
    return UTF8FromUTF16(str);
}

std::u16string CStringEncode::utf8_to_utf16(std::string const& str, std::locale const& loc /*= std::locale()*/) {
    assert(CStringEncode::m_bInitLocale == true);

    typedef std::codecvt<char16_t, char, std::mbstate_t> codecvt_t;

    codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
    std::mbstate_t state = std::mbstate_t(0);
    std::vector<char16_t> buf(str.size() + 1);
    char const* in_next = str.c_str();
    char16_t* out_next = &buf[0];
    std::codecvt_base::result r = codecvt.in(state, 
                                             str.c_str(), str.c_str() + str.size(), in_next, 
                                             &buf[0], &buf[0] + buf.size(), out_next);

    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert string to u16string");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert string to u16string");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert string to u16string");
    case std::codecvt_base::ok:      break; // no problem.
    }

    return std::u16string(&buf[0]);
}

std::string CStringEncode::utf16_to_utf8( std::u16string const& str, std::locale const& loc /*= std::locale()*/ ) {
    assert(CStringEncode::m_bInitLocale == true);
    
    typedef std::codecvt<char16_t, char, std::mbstate_t> codecvt_t;

    codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
    std::mbstate_t state = std::mbstate_t();
    std::vector<char> buf((str.size() + 1) * codecvt.max_length());
    char16_t const* in_next = str.c_str();
    char* out_next = &buf[0];
    std::codecvt_base::result r = codecvt.out(state, 
                                              str.c_str(), str.c_str() + str.size(), in_next, 
                                              &buf[0], &buf[0] + buf.size(), out_next);
    
    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert u16string to string");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert u16string to string");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert u16string to string");
    case std::codecvt_base::ok:      break; // no problem.
    }

    return std::string(&buf[0]);
}

std::u32string CStringEncode::utf8_to_utf32(std::string const& str, std::locale const& loc /*= std::locale()*/) {
    assert(CStringEncode::m_bInitLocale == true);

    typedef std::codecvt<char32_t, char, std::mbstate_t> codecvt_t;

    codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
    std::mbstate_t state = std::mbstate_t(0);
    std::vector<char32_t> buf(str.size() + 1);
    char const* in_next = str.c_str();
    char32_t* out_next = &buf[0];
    std::codecvt_base::result r = codecvt.in(state, 
                                             str.c_str(), str.c_str() + str.size(), in_next, 
                                             &buf[0], &buf[0] + buf.size(), out_next);

    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert string to u32string");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert string to u32string");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert string to u32string");
    case std::codecvt_base::ok:      break; // no problem.
    }

    return std::u32string(&buf[0]);
}

std::string CStringEncode::utf32_to_utf8( std::u32string const& str, std::locale const& loc /*= std::locale()*/ ) {
    assert(CStringEncode::m_bInitLocale == true);

    typedef std::codecvt<char32_t, char, std::mbstate_t> codecvt_t;

    codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
    std::mbstate_t state = std::mbstate_t();
    std::vector<char> buf((str.size() + 1) * codecvt.max_length());
    char32_t const* in_next = str.c_str();
    char* out_next = &buf[0];
    std::codecvt_base::result r = codecvt.out(state, 
                                              str.c_str(), str.c_str() + str.size(), in_next, 
                                              &buf[0], &buf[0] + buf.size(), out_next);

    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert u32string to string");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert u32string to string");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert u32string to string");
    case std::codecvt_base::ok:      break; // no problem.
    }

    return std::string(&buf[0]);
}

std::wstring CStringEncode::mbs_to_wcs(std::string const& str, std::locale const& loc /*= std::locale()*/)
{
  assert(CStringEncode::m_bInitLocale == true);

  typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;

  codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
  std::mbstate_t state = std::mbstate_t(0);
  std::vector<wchar_t> buf(str.size() + 1);
  char const* in_next = str.c_str();
  wchar_t* out_next = &buf[0];
  std::codecvt_base::result r = codecvt.in(state, 
                                         str.c_str(), str.c_str() + str.size(), in_next, 
                       &buf[0], &buf[0] + buf.size(), out_next);

    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert string to wstring");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert string to wstring");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert string to wstring");
    case std::codecvt_base::ok:      break; // no problem.
    }

  return std::wstring(&buf[0]);
}

std::string CStringEncode::wcs_to_mbs(std::wstring const& str, std::locale const& loc /*= std::locale()*/)
{
  assert(CStringEncode::m_bInitLocale == true);

  typedef std::codecvt<wchar_t, char, std::mbstate_t> codecvt_t;

  codecvt_t const& codecvt = std::use_facet<codecvt_t>(loc);
  std::mbstate_t state = std::mbstate_t();
  std::vector<char> buf((str.size() + 1) * codecvt.max_length());
  wchar_t const* in_next = str.c_str();
  char* out_next = &buf[0];
  std::codecvt_base::result r = codecvt.out(state, 
                                          str.c_str(), str.c_str() + str.size(), in_next, 
                        &buf[0], &buf[0] + buf.size(), out_next);

    switch (r) {
    case std::codecvt_base::error:   throw std::runtime_error("error: can't convert wstring to string");
    case std::codecvt_base::partial: throw std::runtime_error("partial: can't convert wstring to string");
    case std::codecvt_base::noconv:  throw std::runtime_error("noconv: not need convert wstring to string");
    case std::codecvt_base::ok:      break; // no problem.
    }

  return std::string(&buf[0]);
}

std::wstring CStringEncode::mbs_to_wcs2(std::string const& str, UINT codePage)
{
  int length = MultiByteToWideChar(codePage, 0, (LPCSTR)str.c_str(), -1, NULL, 0);

  std::vector<wchar_t> buf( length );
  int result = MultiByteToWideChar(codePage, 0, str.c_str(), -1, &buf[0], buf.size());
  if (0 == result)
  {
    throw std::runtime_error(CUtil::GetLastErrorMessageA().c_str());
  }

  std::wstring retval( &buf[0] );
  return retval;
}

std::string CStringEncode::wcs_to_mbs2(std::wstring const& str, UINT codePage)
{
  int length = WideCharToMultiByte(codePage, 0, str.c_str(), -1, NULL, 0, NULL, NULL);

  std::vector<char> buf(length);
  int result = WideCharToMultiByte(codePage, 0, str.c_str(), -1, &buf[0], buf.size(), NULL, NULL);
  if (0 == result)
  {
    throw std::runtime_error(CUtil::GetLastErrorMessageA().c_str());
  }

  std::string retval( &buf[0] );
  return retval;
}

std::wstring CStringEncode::mbs_to_wcs3(std::string const& str)
{
  // TODO: 검증 안됬음. 테스트 해볼 것.
  int length = str.size();
  std::vector<wchar_t> buf(length);
  size_t i;
  mbstowcs_s(&i, &buf[0], (size_t)buf.size(), str.c_str(), strlen(str.c_str()));

  std::wstring retVal(&buf[0]);
  return retVal;
}

std::string CStringEncode::wcs_to_mbs3(std::wstring const& str)
{
  // TODO: 검증 안됬음. 테스트 해볼 것.
  int length = str.size();
  std::vector<char> buf(length*4);
  size_t i;
  wcstombs_s(&i, &buf[0], (size_t)buf.size(), str.c_str(), wcslen(str.c_str()));

  std::string retVal(&buf[0]);
  return retVal;
}

//////////////////////////////////////////////////////////////////////////
int CStringEncode::UTF8ToUnicode( LPBYTE src, LPWSTR dst )
{
  int p = 0;
  int q = 0;
  wchar_t d;

  while (src[p]) 
  {
    if( (src[p] & 0xE0) == 0xE0 ) 
    {
      d = ((src[p] & 0x0f) << 12) | ((src[p+1] & 0x3F) << 6) | (src[p+2] & 0x3F);
      p += 3;
    } 
    else if( (src[p]&0xC0) ==0xC0 ) 
    {
      d = ((src[p]&0x1F)<<6) | (src[p+1]&0x3F);
      p += 2;
    } 
    else 
    {
      d = src[p]&0x7F;
      ++p;
    }

    dst[q] = d;
    ++q;
  }

  dst[q] = NULL;
  return q;
}

int CStringEncode::UnicodeToUTF8( LPCWSTR src, LPBYTE dst /* = NULL*/ )
{
  int p = 0;
  int q = 0;

  if (dst)
  {
    while( src[p] ) 
    {
      if( src[p] < 0x0080 )
      {
        dst[q++] = src[p]&0x7F;
      } 
      else if( src[p] < 0x0800 )
      {
        dst[q++] = 0xC0 | (BYTE)((src[p] & 0x07c0) >> 6);
        dst[q++] = 0x80 | (BYTE)(src[p] & 0x003f);
      } 
      else 
      {
        dst[q++] = 0xE0 | (BYTE)((src[p] & 0xf000) >> 12);
        dst[q++] = 0x80 | (BYTE)((src[p] & 0x0fc0) >> 6);
        dst[q++] = 0x80 | (BYTE)(src[p] & 0x003f);
      }
      p++;
    }
  }
  else
  {
    while (src[p]) 
    {
      if (src[p] < 0x0080)		q++;
      else if (src[p] < 0x0800)	q += 2;
      else 						q += 3;

      p++;
    }
  }

  return q;
}

int CStringEncode::GetUTF8LengthFromUTF16String(const wchar_t* szText, unsigned int nLen)
{
  int nResult = 0;

  for (unsigned int i = 0; i < nLen && szText[i]; ++i)
  {
    nResult += GetUTF8CharLengthFromUTF16(szText[i]);
  }

  return nResult;
}

int CStringEncode::GetUTF8CharLengthFromUTF16(wchar_t ch)
{
  int nUTF8Length = 0;

       if (ch < 0x80)			nUTF8Length = 1;
  else if (ch < 0x800)		nUTF8Length = 2;
  else if (ch < 0x10000)		nUTF8Length = 3;
  else if (ch < 0x200000)		nUTF8Length = 4;
  else if (ch < 0x4000000)	nUTF8Length = 5;

  return nUTF8Length;
}

//////////////////////////////////////////////////////////////////////////

void CStringEncode::UTF8FromUCS2(const wchar_t* uptr, unsigned int tlen, char* putf, unsigned int len)
{
  int k = 0;
  for (unsigned int i = 0; i < tlen && uptr[i]; i++) {
    unsigned int uch = uptr[i];
    if (uch < 0x80) {
      putf[k++] = static_cast<char>(uch);
    } 
        else if (uch < 0x800) {
      putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    } 
        else {
      putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
      putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    }
  }

  putf[len] = '\0';
}

unsigned int CStringEncode::UCS2Length(const char* s, unsigned int len) 
{
  unsigned int ulen = 0;
  for (unsigned int i = 0; i < len; ++i) {
    unsigned char ch = static_cast<unsigned char>(s[i]);
    if ((ch < 0x80) || (ch > (0x80 + 0x40))) {
      ulen++;
        }
  }

  return ulen;
}

unsigned int CStringEncode::UCS2FromUTF8(const char* s, unsigned int len, wchar_t* tbuf, unsigned int tlen)
{
  unsigned int ui = 0;
  const unsigned char *us = reinterpret_cast<const unsigned char *>(s);
  unsigned int i = 0;
  while ((i < len) && (ui < tlen)) {
    unsigned char ch = us[i++];
    if (ch < 0x80) {
      tbuf[ui] = ch;
    } 
        else if (ch < 0x80 + 0x40 + 0x20) {
      tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    } 
        else {
      tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    }

    ui++;
  }

  return ui;
}

void CStringEncode::UTF8FromUTF16(const wchar_t* uptr, unsigned int tlen, char* putf, unsigned int len) 
{
  int k = 0;
  for (unsigned int i = 0; i < tlen && uptr[i];) {
    unsigned int uch = uptr[i];
    if (uch < 0x80) {
      putf[k++] = static_cast<char>(uch);
    } 
        else if (uch < 0x800) {
      putf[k++] = static_cast<char>(0xC0 | (uch >> 6));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    } 
        else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
        // Half a surrogate pair
        i++;
        unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (uptr[i] & 0x3ff);
        putf[k++] = static_cast<char>(0xF0 | (xch >> 18));
        putf[k++] = static_cast<char>(0x80 | (xch >> 12) & 0x3f);
        putf[k++] = static_cast<char>(0x80 | ((xch >> 6) & 0x3f));
        putf[k++] = static_cast<char>(0x80 | (xch & 0x3f));
    } 
        else {
      putf[k++] = static_cast<char>(0xE0 | (uch >> 12));
      putf[k++] = static_cast<char>(0x80 | ((uch >> 6) & 0x3f));
      putf[k++] = static_cast<char>(0x80 | (uch & 0x3f));
    }
    i++;
  }
  putf[len] = '\0';
}


unsigned int CStringEncode::UTF16FromUTF8(const char* s, unsigned int len, wchar_t* tbuf, unsigned int tlen) 
{
  unsigned int ui = 0;
  const unsigned char* us = reinterpret_cast<const unsigned char *>(s);
  unsigned int i = 0;
  while ((i < len) && (ui < tlen)) {
    unsigned char ch = us[i++];
    if (ch < 0x80) {
      tbuf[ui] = ch;
    } 
        else if (ch < 0x80 + 0x40 + 0x20) {
      tbuf[ui] = static_cast<wchar_t>((ch & 0x1F) << 6);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    } 
        else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
      tbuf[ui] = static_cast<wchar_t>((ch & 0xF) << 12);
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + ((ch & 0x7F) << 6));
      ch = us[i++];
      tbuf[ui] = static_cast<wchar_t>(tbuf[ui] + (ch & 0x7F));
    } 
        else {
      // Outside the BMP so need two surrogates
      int val = (ch & 0x7) << 18;
      ch = us[i++];
      val += (ch & 0x3F) << 12;
      ch = us[i++];
      val += (ch & 0x3F) << 6;
      ch = us[i++];
      val += (ch & 0x3F);
      tbuf[ui] = static_cast<wchar_t>(((val - 0x10000) >> 10) + SURROGATE_LEAD_FIRST);
      ui++;
      tbuf[ui] = static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST);
    }
    ui++;
  }

  return ui;
}

unsigned int CStringEncode::GetUTF16LengthFromUTF8String(const char* s, unsigned int len) 
{
  unsigned int ulen = 0;
  unsigned int charLen;
  for (unsigned int i = 0; i < len;) {
    unsigned char ch = static_cast<unsigned char>(s[i]);
    if (ch < 0x80) {
      charLen = 1;
    } else if (ch < 0x80 + 0x40 + 0x20) {
      charLen = 2;
    } else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
      charLen = 3;
    } else {
      charLen = 4;
      ulen++; // 여기서부터는 2개를 조합하기에 더 필요.
    }

    i += charLen;
    ulen++;
  }

  return ulen;
}

//////////////////////////////////////////////////////////////////////////

std::string CStringEncode::UTF8FromUTF16(const std::wstring& strUTF16) 
{
    int k = 0;
    std::vector<char> vecUTF8;
    vecUTF8.reserve(GetUTF8LengthFromUTF16String(strUTF16.c_str(), strUTF16.size()) + 1); // +1 is for null

    for (unsigned int i = 0; i < strUTF16.size() && strUTF16[i];) {
        unsigned int uch = strUTF16[i];
        if (uch < 0x80) {
            vecUTF8.push_back(static_cast<char>(uch));
        } 
        else if (uch < 0x800) {
            vecUTF8.push_back(static_cast<char>(0xC0 | (uch >> 6)));
            vecUTF8.push_back(static_cast<char>(0x80 | (uch & 0x3f)));
        } 
        else if ((uch >= SURROGATE_LEAD_FIRST) && (uch <= SURROGATE_TRAIL_LAST)) {
            // Half a surrogate pair
            ++i;
            unsigned int xch = 0x10000 + ((uch & 0x3ff) << 10) + (strUTF16[i] & 0x3ff);
            vecUTF8.push_back(static_cast<char>(0xF0 | (xch >> 18)));
            vecUTF8.push_back(static_cast<char>(0x80 | (xch >> 12) & 0x3f));
            vecUTF8.push_back(static_cast<char>(0x80 | ((xch >> 6) & 0x3f)));
            vecUTF8.push_back(static_cast<char>(0x80 | (xch & 0x3f)));
        } 
        else {
            vecUTF8.push_back(static_cast<char>(0xE0 | (uch >> 12)));
            vecUTF8.push_back(static_cast<char>(0x80 | ((uch >> 6) & 0x3f)));
            vecUTF8.push_back(static_cast<char>(0x80 | (uch & 0x3f)));
        }

        ++i;
    }

    vecUTF8.push_back('\0');

    return std::string(&vecUTF8[0]);
}


std::wstring CStringEncode::UTF16FromUTF8(const std::string& strUTF8) 
{
    const unsigned char* us = reinterpret_cast<const unsigned char *>(strUTF8.c_str());

    std::vector<wchar_t> vecUTF16;
    vecUTF16.reserve(GetUTF16LengthFromUTF8String(strUTF8.c_str(), strUTF8.size()) + 1); // +1 is for null
    wchar_t temp;
    for (unsigned int i = 0; i < strUTF8.size();) {
        unsigned char ch = us[i++];
        if (ch < 0x80) {
            vecUTF16.push_back(ch);
        } 
        else if (ch < 0x80 + 0x40 + 0x20) {
            temp = static_cast<wchar_t>((ch & 0x1F) << 6);
            ch = us[i++];
            vecUTF16.push_back(static_cast<wchar_t>(temp + (ch & 0x7F)));
        } 
        else if (ch < 0x80 + 0x40 + 0x20 + 0x10) {
            temp = static_cast<wchar_t>((ch & 0xF) << 12);
            ch = us[i++];
            temp = static_cast<wchar_t>(temp + ((ch & 0x7F) << 6));
            ch = us[i++];
            vecUTF16.push_back(static_cast<wchar_t>(temp + (ch & 0x7F)));
        } 
        else {
            // Outside the BMP so need two surrogates
            int val = (ch & 0x7) << 18;
            ch = us[i++];
            val += (ch & 0x3F) << 12;
            ch = us[i++];
            val += (ch & 0x3F) << 6;
            ch = us[i++];
            val += (ch & 0x3F);
            vecUTF16.push_back(static_cast<wchar_t>(((val - 0x10000) >> 10) + SURROGATE_LEAD_FIRST));
            vecUTF16.push_back(static_cast<wchar_t>((val & 0x3ff) + SURROGATE_TRAIL_FIRST));
        }
    }

    vecUTF16.push_back('\0');

    return std::wstring(&vecUTF16[0]);
}