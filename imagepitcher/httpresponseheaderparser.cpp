#include "StdAfx.h"
#include "httpresponseheaderparser.h"
#include "urlencode.h"

using namespace std;

HttpResponseHeaderParser::HttpResponseHeaderParser()
  : m_bParsed(false)
{
}

HttpResponseHeaderParser::~HttpResponseHeaderParser(void)
{
}

void HttpResponseHeaderParser::splitHeader(const string& strHttpHeader)
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
      strPrefix = "RES";
      strContent = strLine;
    }

    m_mapHeader.insert(make_pair(strPrefix, strContent));
  }
}


/*
HTTP/1.1 200 OK
Date: Sun, 29 Apr 2012 05:34:51 GMT
Server: Apache
Keep-Alive: timeout=5, max=100
Connection: Keep-Alive
Transfer-Encoding: chunked
Content-Type: text/html
*/

bool HttpResponseHeaderParser::parse(const string& strHttpHeader)
{
  // HTTP에 의한 요청인지 확인.
  if (strHttpHeader.find("HTTP/") == string::npos) {
    return false;
  }

  splitHeader(strHttpHeader);

  int nCurPos = 0, nNextPos = 0;

  // import Response code.
  nCurPos = m_mapHeader["RES"].find(" ");
  stringstream resString(m_mapHeader["RES"].substr(nCurPos+1));
  resString >> ResponseCode;
  ++nCurPos;

  m_bParsed = true;

  return true;
}