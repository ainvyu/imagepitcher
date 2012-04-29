#include "StdAfx.h"
#include "httprequestheaderparser.h"
#include "urlencode.h"

using namespace std;

HttpRequestHeaderParser::HttpRequestHeaderParser()
  : m_bParsed(false)
  , m_nRangeStart(0)
  , m_nRangeEnd(0)
{
}

HttpRequestHeaderParser::~HttpRequestHeaderParser(void)
{
}

void HttpRequestHeaderParser::SplitHeader(const string& strHttpHeader)
{
  vector<string> vecHeaderToken;
  vecHeaderToken.reserve(17);
  CStringUtil::Tokenize(strHttpHeader, vecHeaderToken, "\r\n");

  string strPrefix, strContent;
  for (auto it = vecHeaderToken.cbegin(), e = vecHeaderToken.cend(); it != e; 
      ++it) {
    const string& strLine = (*it);
    if (strLine.empty()) continue;

    int nSep = strLine.find(": ");
    if (nSep != string::npos) {
      strPrefix = strLine.substr(0, nSep);
      strContent = strLine.substr(nSep + 2, strLine.size() - (nSep + 2));
    }
    else {
      strPrefix = "REQ";
      strContent = strLine;
    }

    m_mapHeader.insert(make_pair(strPrefix, strContent));
  }
}


/*
GET /9cde7765-72c4-46d4-b109-b967098f99c9/boost_1_43 HTTP/1.1
Host: 127.0.0.1:81
Connection: keep-alive
Accept: application/xml,application/xhtml+xml,text/html;q=0.9,text/plain;q=0.8,image/png,* / *;q=0.5
User-Agent: Mozilla/5.0 (Windows; U; Windows NT 6.1; en-US) AppleWebKit/534.0 (KHTML, like Gecko) Chrome/6.0.408.1 Safari/534.0
Accept-Encoding: gzip,deflate,sdch
Accept-Language: ko-KR,ko;q=0.8,en-US;q=0.6,en;q=0.4
Accept-Charset: windows-949,utf-8;q=0.7,*;q=0.3
Cookie: CodeSnippetContainerLang=Visual%20Basic
*/

/*
이어받기시 예제
GET /ubuntu-releases/9.10/ubuntu-9.10-alternate-amd64.iso HTTP/1.1
Accept: * / *
Accept-Encoding: gzip, deflate
Range: bytes=65816424-
Unless-Modified-Since: Tue, 27 Oct 2009 17:17:43 GMT
If-Range: "-304928825"
User-Agent: Mozilla/4.0 (compatible; MSIE 7.0; Windows NT 6.1; WOW64; Trident/4.0; chromeframe; SLCC2; .NET CLR 2.0.50727; .NET CLR 3.5.30729; .NET CLR 3.0.30729; Media Center PC 6.0; .NET CLR 1.1.4322; Zune 4.0; .NET4.0C; .NET4.0E)
Host: ftp.daum.net
Connection: Keep-Alive
*/

bool HttpRequestHeaderParser::Parse(const string& strHttpHeader)
{
  // HTTP에 의한 요청인지 확인.
  if (strHttpHeader.find("HTTP/") == string::npos)
  {
    return false;
  }

  SplitHeader(strHttpHeader);

  int nCurPos = 0, nNextPos = 0;

  nCurPos = m_mapHeader["REQ"].find(" /");
  m_strMethod = strHttpHeader.substr(0, nCurPos);
  ++nCurPos;

  nNextPos = m_mapHeader["REQ"].find(" HTTP"); // 요청 파일 이름 확인
  m_strRequestAddress = strHttpHeader.substr(nCurPos, nNextPos-nCurPos);
  nCurPos = nNextPos + 1;

  /// Import Referer
  CMapStringToString::const_iterator it = m_mapHeader.find("Referer");
  if (it != m_mapHeader.end()) {
    m_strReferer = (*it).second;
  }

  if (m_strMethod == "GET") {
    CMapStringToString::const_iterator it;
    it = m_mapHeader.find("Range");
    if (it != m_mapHeader.end())
    {
      const string& strRange = (*it).second;
      int nStartPos = strRange.find("=");
      int nRangePos = strRange.find("-", nStartPos);

      m_nRangeStart = CStringUtil::StrToInt64(
        strRange.substr(nStartPos+1, nRangePos - (nStartPos+1))
        );
      m_nRangeEnd   = CStringUtil::StrToInt64(
        strRange.substr(nRangePos+1, strRange.size() - (nRangePos+1))
        );
    }

    string strDecodePath = CUrlEncode::URLDecode(m_strRequestAddress.c_str());
    //m_strRequestAddress = CStringEncode::utf8_to_multibyte(strDecodePath);
    m_strRequestAddress = strDecodePath;

    m_bParsed = true;
  }
  else if (m_strMethod == "POST") {
    CMapStringToString::const_iterator it;
    it = m_mapHeader.find("Content-Length");
    if (it != m_mapHeader.end()) {
      std::string strContentLength = (*it).second;
      m_nContentLength = CStringUtil::StrToInt64(strContentLength);
    }

    it = m_mapHeader.find("Content-Type");
    if (it != m_mapHeader.end()) {
      std::string strContentType = (*it).second;

      std::string expr("(.+?); boundary=(.+)");
      std::regex rgx(expr, regex::icase);
      std::smatch match;

      if (std::regex_search(strContentType, match, rgx)) {
        m_strContentType = match[1];
        m_strBoundary = match[2];
      }
    }

    string strDecodePath = CUrlEncode::URLDecode(m_strRequestAddress.c_str());
    //m_strRequestAddress = CStringEncode::utf8_to_multibyte(strDecodePath);
    m_strRequestAddress = strDecodePath;

    m_bParsed = true;
  }
  
  return true;
}