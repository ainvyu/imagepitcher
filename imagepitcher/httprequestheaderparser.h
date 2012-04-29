#pragma once

class HttpRequestHeaderParser
{
public:
  HttpRequestHeaderParser(void);
  ~HttpRequestHeaderParser(void);

  bool isParsed() const { return m_bParsed; }
  bool Parse(const std::string& strHttpHeader);
  void SplitHeader(const std::string& strHttpHeader);
  const std::string& GetMethod() const	     { assert(m_bParsed); return m_strMethod; }
  const std::string& GetRequestAddress() const { assert(m_bParsed); return m_strRequestAddress; }
  const std::string& GetRequestType() const    { assert(m_bParsed); return m_strRequestType; }
  const std::string& GetHost() const           { assert(m_bParsed); return m_strHost; }
  LONGLONG GetStartRange() const { return m_nRangeStart; }
  LONGLONG GetEndRange() const { return m_nRangeEnd; }
  
  // for POST
  LONGLONG GetContentLength() const { assert(m_bParsed); return m_nContentLength; }
  const std::string& GetContentType() const { assert(m_bParsed); return m_strContentType; }
  const std::string& GetBoundary() const { assert(m_bParsed); return m_strBoundary; }

private:
  bool m_bParsed;

  typedef std::map<std::string, std::string> CMapStringToString;
  std::string m_strMethod;
  std::string m_strHost;
  std::string m_strReferer;
  CMapStringToString m_mapHeader;

  // for GET
  std::string m_strRequestAddress;
  std::string m_strRequestType;
  LONGLONG m_nRangeStart;
  LONGLONG m_nRangeEnd;

  // for POST
  LONGLONG m_nContentLength;
  std::string m_strContentType;
  std::string m_strBoundary;
};