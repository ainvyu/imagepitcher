#include "StdAfx.h"

#include "UrlEncode.h"

using namespace std;

// �ظ��ϸ� UTF-8 ���ڸ� �ѱ�°� ����.
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

// ������ UTF-8 ���ڿ��� ���ƿͼ� Decode �ȴ�.
// ������ Decode�� ������� CStringEncode::utf8_to_multibyte�� ����Ͽ� �����Ͽ� Ǯ���ش�.
string CUrlEncode::URLDecode(const char* strSrc)
{
  if (!strSrc)
    return NULL;

  // ���ڿ� ���鼭 decode
  string strDest;
  for (int i = 0; strSrc[i]; i++) 
  {
    switch (strSrc[i]) 
    {
    case '+':
      // '+' ���ڴ� ��������
      strDest += ' ';
      break;

    case '%':
      // % ���ڰ� ������ ���� �ΰ� ���ڸ� ����
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