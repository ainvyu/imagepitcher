#pragma once

struct PostItem {
  PostItem(std::string _name, std::string _sep, std::string _val)
    : name(_name)
    , separator(_sep)
    , value(_val)
  {}

  std::string name;
  std::string separator;
  std::string value;
};

class CHttpPost
{
public:
  CHttpPost(void);
  virtual ~CHttpPost(void);

  void SetBoundary(const std::string& _boundary) { boundary_ = _boundary; }
  void SetURL(const std::string& _url) { url_ = _url; }
  void SetPath(const std::string& _path) { path_ = _path; }
  void SetContent(const std::string& _content) { content_ = _content; }
  void SetUserAgent(const std::string& _userAgent) { userAgent_ = _userAgent; }
  void SetPort(const std::string& _port) { port_ = _port; }
  void AddHeader(const std::string& _section, PostItem item);
  void RegisterSectionSeparator(const std::string& _section, const std::string& _sep);

  bool DoPost(void);
  const std::string& GetResponse() const { return response_; }

private:
  typedef std::unordered_map<std::string, std::list<PostItem>> headerHash;
  headerHash customHeader_;
  typedef std::unordered_map<std::string, std::string> sectionSepHash;
  sectionSepHash sectionSepHash_;

  boost::asio::io_service io_service_;
  std::string content_;
  std::string url_;
  std::string path_;
  std::string boundary_;
  std::string userAgent_;
  std::string port_;

  std::string response_;
};