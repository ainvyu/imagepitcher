#pragma once

class HttpResponseHeaderParser
{
public:
  HttpResponseHeaderParser(void);
  ~HttpResponseHeaderParser(void);

  bool isParsed() const { return m_bParsed; }
  bool parse(const std::string& strHttpHeader);

  int getResponseCode()const { assert(m_bParsed); return ResponseCode; }

private:
  void splitHeader(const std::string& strHttpHeader);

private:
  bool m_bParsed;

  typedef std::map<std::string, std::string> CMapStringToString;
  std::string m_strMethod;
  std::string m_strHost;
  std::string m_strReferer;
  CMapStringToString m_mapHeader;

  int ResponseCode;
};