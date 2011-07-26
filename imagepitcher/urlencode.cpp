#include "StdAfx.h"

#include "UrlEncode.h"

using namespace std;

// 왠만하면 UTF-8 문자를 넘기는게 좋다.
string CUrlEncode::URLEncode(const char* strSrc, 
                             bool bWebBrowserFriendly,
                             bool bUpperCase) 
{
  if (strSrc == NULL) 
  {
    return "";
  }

  char buf[2+1];
  vector<char> arrEnc(strlen(strSrc) * 3 + 1);
  unsigned char c;

  for(int i = 0, j = 0; strSrc[i]; ++i) 
  {
    c = (unsigned char)strSrc[i];
    if (isalnum(c))
    {
      arrEnc[j++] = c;
    }
    else if ((c == '.') || 
             (c == '-') || 
             (c == '_') || 
             (c == '~'))
    {
      arrEnc[j++] = c;
    }
    else
    {
      if (bWebBrowserFriendly) {
        if ((c == '.') || 
            (c == '-') || 
            (c == '_') || 
            (c == '~') ||
            (c == '@') || 
            (c == '/') || 
            (c == '\\') || 
            (c == ':') || 
            (c == '[') || (c == ']') || 
            (c == '(') || (c == ')') || 
            (c == '!') || (c == '?'))
        {
          arrEnc[j++] = c;
          continue;
        }
      }

      if (bUpperCase)
        sprintf(buf, "%02X", c);
      else
        sprintf(buf, "%02x", c);

      arrEnc[j++] = '%';
      arrEnc[j++] = buf[0];
      arrEnc[j++] = buf[1];
    } 
  }

  return string(&arrEnc[0]);
}

std::string CUrlEncode::URLEncode(const std::string& strSrc, 
                                  bool bWebBrowserFriendly /*= false*/, 
                                  bool bUpperCase /*= true*/ )
{
  return CUrlEncode::URLEncode(strSrc.c_str(), 
                               bWebBrowserFriendly, 
                               bUpperCase);
}

char CUrlEncode::x2c(char hex_up, char hex_low) 
{
  char digit;

  digit = 16 * (hex_up >= 'A' ? ((hex_up & 0xdf) - 'A') + 10 : (hex_up - '0'));
  digit += (hex_low >= 'A' ? ((hex_low & 0xdf) - 'A') + 10 : (hex_low - '0'));
  return (digit);
}

// 보통은 UTF-8 문자열이 날아와서 Decode 된다.
// 때문에 Decode된 결과물은 CStringEncode::utf8_to_multibyte를 사용하여 적절하여 풀어준다.
string CUrlEncode::URLDecode(const char* strSrc)
{
  if (!strSrc)
    return NULL;

  // 문자열 돌면서 decode
  string strDest;
  for (int i = 0; strSrc[i]; i++) 
  {
    switch (strSrc[i]) 
    {
    case '+':
      // '+' 문자는 공백으로
      strDest += ' ';
      break;

    case '%':
      // % 문자가 나오면 뒤의 두개 문자를 결합
      strDest += x2c(strSrc[i + 1], strSrc[i + 2]);
      i += 2;
      break;

    default:
      strDest += strSrc[i];
      break;
    }
  }

  return strDest;
}